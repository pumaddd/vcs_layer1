/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/thread/thread.h>
#include "file_source_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <stdio.h>
#include <fstream>

// win32 (mingw/msvc) specific
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef O_BINARY
#define	OUR_O_BINARY O_BINARY
#else
#define	OUR_O_BINARY 0
#endif
// should be handled via configure
#ifdef O_LARGEFILE
#define	OUR_O_LARGEFILE	O_LARGEFILE
#else
#define	OUR_O_LARGEFILE 0
#endif

namespace gr {
namespace gsm {

file_source::sptr file_source::make(size_t itemsize,
        uint64_t offset, std::vector<int> arfcn, std::vector<int> flag, uint64_t index) {
    return gnuradio::get_initial_sptr(
            new file_source_impl(itemsize, offset, arfcn, flag, index));
}

file_source_impl::file_source_impl(size_t itemsize,
        uint64_t offset, std::vector<int> arfcn, std::vector<int> flag, uint64_t index) :
        sync_block("file_source", io_signature::make(0, 0, 0),
                io_signature::make(1, 1, itemsize)), d_itemsize(itemsize), d_fp(
                0), d_new_fp(0), d_updated(false), d_arfcn(arfcn), d_flag_arfcn(flag), d_index(index){
        d_table_name = d_grd_config.get_table_name();
#ifdef SQLITE3
      int rc = sqlite3_open("/etc/data/file.db", &fileinfo);

      if( rc ) {
          fprintf(stderr, "file_source_impl:Can't open database: %s\n", sqlite3_errmsg(fileinfo));
      } else {
          fprintf(stderr, "file_source_impl:Opened database successfully\n");
       }
#else
        mysql_driver = get_driver_instance();
        mysql_con = mysql_driver->connect(d_grd_config.get_database(), d_grd_config.get_username(), d_grd_config.get_password());
        /* Connect to the MySQL test database */
        mysql_con->setSchema("file.db");
        mysql_stmt = mysql_con->createStatement();
#endif

      d_time_scan = d_grd_config.get_timescan();
      d_osr = d_grd_config.get_osr();
      d_arfcn.push_back(0);
      l1_capture_info(d_index);

    if(d_arfcn[0] >=512)
          d_sample_offset = d_sample_rate_out_1800 * offset / (1625000 * d_osr / 6);
      if(d_arfcn[0] <=124)
          d_sample_offset = d_sample_rate_out_900 * offset / (1625000 * d_osr / 6);
    int64_t loop = d_sample_offset / 268435456;
    int _offset = d_sample_offset % 268435456;
    file_info.ID = 0;
    d_id = 0;
    d_update_file = false;
    d_res = 0;
    d_offset = 0;
    d_flag_start = true;
    d_i = 0;
    d_count_sample = 0;
    find_fc_from_arfcn(d_arfcn[d_i]);
    if(loop < 0)
        loop = 0;
    d_res = find_file_start(loop);
    
    // if(d_res == -1) {
    // 	while(deadLine) {
    // 		d_res = find_file(fileinfo, d_fc_file_current);
    // 		if(d_res != -1)
    // 			break;	
    // 		sleep(1);
    // 		deadLine --;
    // 	}
    // }

    // std::ifstream in(file_info.filepath.c_str(), std::ifstream::ate | std::ifstream::binary);
    // uint64_t real_file_size = in.tellg();
    // in.close();

    // if(real_file_size < (_offset * d_itemsize)) {
    // 	int delta_time = 10 * (_offset - real_file_size/d_itemsize) / file_info.bwidth;
    // 	sleep(delta_time);
    // }
    d_real_offset = get_capture_offset(d_fc_file_current);
    if(d_res == -1)
        std::cout<<"Init offset out of range \t current_offset: "<<offset<<"\t"<<"real offset\t"<<get_real_offset()<<std::endl;
    if(d_res != -1)
        set_file(file_info.filepath.c_str(), _offset);
    message_port_register_out(pmt::mp("message_out"));
    message_port_register_in(pmt::mp("message_in"));
    set_msg_handler(pmt::mp("message_in"),
    boost::bind(&file_source_impl::handle_msg, this, _1));
    set_output_multiple(4000);
}

file_source_impl::~file_source_impl() {
    if (d_fp)
        fclose((FILE*) d_fp);
    if (d_new_fp)
        fclose((FILE*) d_new_fp);
#ifdef SQLITE3
        sqlite3_close(fileinfo);
#else
        mysql_con->close();
        mysql_stmt->close();
        delete mysql_con;
        delete mysql_stmt;
#endif

}

bool file_source_impl::seek(long seek_point, int whence) {
    return fseek((FILE*) d_fp, seek_point * d_itemsize, whence) == 0;
}

void file_source_impl::open(const char *filename) {
    // obtain exclusive access for duration of this function
    gr::thread::scoped_lock lock(fp_mutex);

    int fd;

    // we use "open" to use to the O_LARGEFILE flag
    if ((fd = ::open(filename, O_RDONLY | OUR_O_LARGEFILE | OUR_O_BINARY))
            < 0) {
        perror(filename);
        throw std::runtime_error("can't open file");
    }

    if (d_new_fp) {
        fclose(d_new_fp);
        d_new_fp = 0;
    }

    if ((d_new_fp = fdopen(fd, "rb")) == NULL) {
        perror(filename);
        ::close(fd);	// don't leak file descriptor if fdopen fails
        throw std::runtime_error("can't open file");
    }
    d_updated = true;
}

void file_source_impl::close() {
    // obtain exclusive access for duration of this function
    gr::thread::scoped_lock lock(fp_mutex);

    if (d_new_fp != NULL) {
        fclose(d_new_fp);
        d_new_fp = NULL;
    }
    d_updated = true;
}

void file_source_impl::do_update() {
    if (d_updated) {
        gr::thread::scoped_lock lock(fp_mutex); // hold while in scope

        if (d_fp)
            fclose(d_fp);

        d_fp = d_new_fp;    // install new file pointer
        d_new_fp = 0;
        d_updated = false;
    }
}

int file_source_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items) {
    char *o = (char*) output_items[0];
    static unsigned char test[2];
    int i;
    int size = noutput_items;
    if(d_flag_start) {
        d_flag_start = false;
        pmt::pmt_t messages = pmt::make_dict();
        if(d_arfcn[d_i] <= 124) {
            messages = dict_add(messages, pmt::string_to_symbol("set_phase_inc"),
            pmt::from_double(-2*M_PI*(d_f_current - d_fc_file_current)/d_sample_rate_out_900));
            messages = dict_add(messages, pmt::string_to_symbol("rx_rate"),
            pmt::from_double(d_sample_rate_out_900));
            if(d_flag_arfcn[d_i] == 0){
                messages = dict_add(messages, pmt::string_to_symbol("set_arfcn"),
                pmt::from_uint64(d_arfcn[d_i]));
            }
            if(d_flag_arfcn[d_i] == 1){
                messages = dict_add(messages, pmt::string_to_symbol("set_arfcn_priority"),
                pmt::from_uint64(d_arfcn[d_i]));
            }
        }
        if(d_arfcn[d_i] >= 512) {
            messages = dict_add(messages, pmt::string_to_symbol("set_phase_inc"),
            pmt::from_double(-2*M_PI*(d_f_current - d_fc_file_current)/d_sample_rate_out_1800));
            messages = dict_add(messages, pmt::string_to_symbol("rx_rate"),
            pmt::from_double(d_sample_rate_out_1800));
            if(d_flag_arfcn[d_i] == 0){
                messages = dict_add(messages, pmt::string_to_symbol("set_arfcn"),
                pmt::from_uint64(d_arfcn[d_i]));
            }
            if(d_flag_arfcn[d_i] == 1){
                messages = dict_add(messages, pmt::string_to_symbol("set_arfcn_priority"),
                pmt::from_uint64(d_arfcn[d_i]));
            }
        }
        pmt::pmt_t klist(pmt::dict_keys(messages));
        for (size_t i = 0; i < pmt::length(klist); i++) {
          pmt::pmt_t k(pmt::nth(i, klist));
          pmt::pmt_t v(pmt::dict_ref(messages, k, pmt::PMT_NIL));
          add_item_tag(0, nitems_written(0), k, v, alias_pmt());
        }

//	    message_port_pub(pmt::intern("message_out"), messages);
    }
    if(d_update_file) {
        d_update_file = false;
        close();
        d_i ++;
        pmt::pmt_t messages = pmt::make_dict();
        if(d_arfcn[d_i] == 0) {
            pmt::pmt_t msg = pmt::make_tuple(pmt::mp("end_of_scan"));
            message_port_pub(pmt::mp("message_out"), msg);
            std::cout<<"[INFO] File source: Scan Done!"<<std::endl;
            return -1;
        }
        if(d_arfcn[d_i] >= 512 && d_arfcn[d_i -1] <=124 && d_arfcn[d_i] > 0) {
            d_sample_offset = d_sample_rate_out_1800 * (d_sample_offset + d_count_sample) / d_sample_rate_out_900;
            messages = dict_add(messages, pmt::string_to_symbol("rx_rate"),
            pmt::from_double(d_sample_rate_out_1800));
        }
        if(d_arfcn[d_i] <= 124 && d_arfcn[d_i -1] >= 512 && d_arfcn[d_i] > 0) {
            d_sample_offset = d_sample_rate_out_900 * (d_sample_offset + d_count_sample) / d_sample_rate_out_1800;
            messages = dict_add(messages, pmt::string_to_symbol("rx_rate"),
            pmt::from_double(d_sample_rate_out_900));
        }
        else
        {
            if(d_arfcn[d_i] > 0)
                d_sample_offset = d_sample_offset + d_count_sample;
        }
        std::cout<<"Switch to arfcn\t"<<d_arfcn[d_i]<<" at sample\t"<<get_current_offset(d_arfcn[d_i])<<std::endl;

        d_count_sample = 0;
        int loop = d_sample_offset / 268435456;
        int _offset = d_sample_offset % 268435456;
        file_info.ID = 0;
        d_id = 0;
        find_fc_from_arfcn(d_arfcn[d_i]);
        d_res = find_file_start(loop);

        // if(d_res == -1) {
        // 	while(deadLine) {
        // 		d_res = find_file(fileinfo, d_fc_file_current);
        // 		if(d_res != -1)
        // 			break;	
        // 		sleep(1);
        // 		deadLine --;
        // 	}
        // }

        // std::ifstream in(file_info.filepath.c_str(), std::ifstream::ate | std::ifstream::binary);
        // uint64_t real_file_size = in.tellg();
        // in.close();
        // if(real_file_size < (_offset * d_itemsize)) {
        // 	int delta_time = 10 * (_offset - real_file_size/d_itemsize) / file_info.bwidth;
        // 	sleep(delta_time);
        // 	std::cout<<"DEBUG delta file size \t"<<_offset - real_file_size/d_itemsize <<std::endl;
        // }
        
        if(d_res != -1)
            set_file(file_info.filepath.c_str(), _offset);
        
        if(d_arfcn[d_i] <= 124) {
            messages = dict_add(messages, pmt::string_to_symbol("set_phase_inc"),
            pmt::from_double(-2*M_PI*(d_f_current - d_fc_file_current)/d_sample_rate_out_900));
            if(d_flag_arfcn[d_i] == 0){

                messages = dict_add(messages, pmt::string_to_symbol("set_arfcn"),
                pmt::from_uint64(d_arfcn[d_i]));
            }
            if(d_flag_arfcn[d_i] == 1){
                messages = dict_add(messages, pmt::string_to_symbol("set_arfcn_priority"),
                pmt::from_uint64(d_arfcn[d_i]));
            }
        }
        if(d_arfcn[d_i] >= 512) {
            messages = dict_add(messages, pmt::string_to_symbol("set_phase_inc"),
            pmt::from_double(-2*M_PI*(d_f_current - d_fc_file_current)/d_sample_rate_out_1800));
            if(d_flag_arfcn[d_i] == 0){
                messages = dict_add(messages, pmt::string_to_symbol("set_arfcn"),
                pmt::from_uint64(d_arfcn[d_i]));

            }
            if(d_flag_arfcn[d_i] == 1){
                messages = dict_add(messages, pmt::string_to_symbol("set_arfcn_priority"),
                pmt::from_uint64(d_arfcn[d_i]));
            }
        }
        pmt::pmt_t klist(pmt::dict_keys(messages));
        for (size_t i = 0; i < pmt::length(klist); i++) {
          pmt::pmt_t k(pmt::nth(i, klist));
          pmt::pmt_t v(pmt::dict_ref(messages, k, pmt::PMT_NIL));
          add_item_tag(0, nitems_written(0), k, v, alias_pmt());
        }
//	    message_port_pub(pmt::intern("message_out"), messages);
    }
    if(d_res) {
        std::cout<<"offset out of range \t current_offset: "<<get_current_offset(d_arfcn[d_i])<<"\t"<<"real offset\t"<<get_real_offset()<<std::endl;
        return -1;
    }

    do_update();       // update d_fp is reqd
    if (d_fp == NULL)
        throw std::runtime_error("work with file not open");

//	gr::thread::scoped_lock lock(fp_mutex); // hold for the rest of this function
    while (size) {
        i = fread(o, d_itemsize, size, (FILE*) d_fp);
        size -= i;
        if(i!=0)
        {
//			printf("size %u , nout %d -- %x%x\n", i, noutput_items, o[i * d_itemsize-2], o[i * d_itemsize-1]);
            test[0] = o[i * d_itemsize-2];
            test[1] = o[i * d_itemsize-1];
        }
        o += i * d_itemsize;

        if (size == 0) {
             // done
            break;
        }
        if (i > 0)			// short read, try again
            continue;

        // We got a zero from fread.  This is either EOF or error.  In
        // any event, if we're in repeat mode, seek back to the beginning
        // of the file and try again, else break
//		printf("----- %2x%2x\n",test[0], test[1]);
//		return -1;
        // int deadLine = 20;
        // int ret = -1; 
        // while (deadLine) {
            // deadLine =  deadLine - 1;
            // std::cout<<"DEBUG wait for next file \t"<<deadLine<<std::endl;
        // ret = find_file(fileinfo, d_fc_file_current);
            // if (ret != -1)
                // break;
                // sleep(1);
        // }
        int ret = find_file(fileinfo, d_fc_file_current);
        if (ret==-1)
            break;
        open(file_info.filepath.c_str());
        do_update();
//		size = noutput_items;
    }

    if (size > 0) {	     		// EOF or error
    if(size == noutput_items) {
      // we didn't read anything; say we're done
      std::cout << "end of file" << std::endl;
      pmt::pmt_t msg = pmt::make_tuple(pmt::mp("end_of_file"));
      message_port_pub(pmt::mp("message_out"), msg);
      d_flag_file = true;
      return -1;
    }
    return noutput_items - size;	// else return partial result
    }
    d_count_sample += noutput_items;
    if(d_arfcn[d_i] <= 124 && d_count_sample == d_time_scan *d_sample_rate_out_900)
        d_update_file = true;
    if(d_arfcn[d_i] >= 512 && d_count_sample == d_time_scan * d_sample_rate_out_1800){
        d_update_file = true;
    }

    return noutput_items;
}

void file_source_impl::set_file(const char* file_name, uint64_t offset_byte) {
    d_flag_file = false;
    open(file_name);
    do_update();
    seek(offset_byte, SEEK_SET);
    }
#ifdef SQLITE3
int
file_source_impl::callback(void *_fileinfo, int argc, char **argv, char **azColName) {
    int i;
    t_file_info *file_info = (t_file_info *)_fileinfo;
    for(i = 0; i<argc; i++) {
        if((strcmp(azColName[i], "filepath")) == 0) {
            file_info->filepath = argv[i];
            std::cout<<"filepath: " << file_info->filepath << std::endl;
       //printf("DEBUG: Name event: %s \n", db.name_event.c_str());
        }
        if((strcmp(azColName[i],"status")) == 0) {

            file_info->status = argv[i];
//			std::cout<<"status: " <<file_info.status << std::endl;
        }
        if((strcmp(azColName[i],"fc")) == 0) {
            file_info->fc = atof(argv[i]);
//			std::cout<<"fc: " << file_info.fc << std::endl;
        }
        if((strcmp(azColName[i], "bwidth")) == 0) {
            file_info->bwidth = atof(argv[i]);
        }
        if((strcmp(azColName[i], "ID")) == 0) {
            file_info->ID = std::strtoull(argv[i],NULL, 0);
//			std::cout<< "ID: " <<file_info.id << std::endl;
        }
    }
return 0;
}
#endif

int
file_source_impl::find_file(sqlite3* database, double fc){
  int ret = -1;
#ifdef SQLITE3
  int rc;
  char *zErrMsg = 0;
  char sql[1000] = {0};
  std::stringstream stream;

  if(d_uplink) {
      sprintf(sql, "SELECT * FROM 'FileInfo' WHERE ID > %d AND status='Up' AND fc=%s Limit 1", file_info.ID, d_fc.c_str());
  }
  else {
      sprintf(sql, "SELECT * FROM 'FileInfo' WHERE ID > %d AND status='Down' AND fc=%s Limit 1", file_info.ID, d_fc.c_str());
  }
  while(1){
  rc = sqlite3_exec(database, sql, callback, (void *)&this->file_info, &zErrMsg);
  if( rc != SQLITE_OK ) {
    usleep(1);
    perror("file_source_impl:Read database fail!!!");
    sqlite3_free(zErrMsg);
  } else {
      break;
  }
  }
#else
  try {
//         sql::Driver *mysql_driver;
//         sql::Connection *mysql_con;
//         sql::Statement *mysql_stmt;
      sql::ResultSet *res = NULL;
      char *zErrMsg = 0;
      char sql[1000] = {0};
      sprintf(sql, "SELECT * FROM `%s` WHERE ID > %d AND status='Down' AND fc=%f AND index_round=%llu Limit 1", d_table_name.c_str(), file_info.ID, fc, d_index);
         res = mysql_stmt->executeQuery(sql);
      if (!res){
          printf("%s\n", sql);
          return -1;
      }
      while (res->next()) {
                ret = 0;
         file_info.filepath =  res->getString(2);
         file_info.status =  res->getString(3);
         file_info.fc =  atof(res->getString(4).c_str());
         file_info.bwidth = atof(res->getString(5).c_str());
         file_info.ID = std::strtoull(res->getString(1).c_str(),NULL, 0);
       }
      delete res;
   }
   catch (sql::SQLException &e) {
       std::cout << "ERROR: SQLException in " << __FILE__;
       std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
       std::cout << "ERROR: " << e.what();
       std::cout << " (MySQL error code: " << e.getErrorCode();
       std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
   }
#endif
  if(d_id == file_info.ID) {
      return -1;
  }
  d_id = file_info.ID;

  return ret;
}

void
file_source_impl::find_fc_from_arfcn(int arfcn) {
    if(arfcn <= 124) {
        double _current_frequency = arfcn2downlink(arfcn);
        for(int i = 0; i < d_fc_file_900.size(); i++){
            //std::cout << "fc_file_900: " << d_fc_file_900[i] << "\tsample_rae: " << d_sample_rate_out_900 << std::endl;
            if(_current_frequency >= d_fc_file_900[i] - d_sample_rate_out_900 / 2 &&
                _current_frequency <= d_fc_file_900[i] + d_sample_rate_out_900 / 2) {
                d_fc_file_current = d_fc_file_900[i];
                d_f_current = _current_frequency;
                break;
            }

        }
    }
    if(arfcn >= 512) {
        double _current_frequency = arfcn2downlink(arfcn);
        for(int i = 0; i < d_fc_file_1800.size(); i++){
            //std::cout << "fc_file_1800: " << d_fc_file_900[i] << "\tsample_rae: " << d_sample_rate_out_1800 << std::endl;
            if(_current_frequency >= d_fc_file_1800[i] - d_sample_rate_out_1800 / 2 &&
                _current_frequency <= d_fc_file_1800[i] + d_sample_rate_out_1800 / 2) {
                d_fc_file_current = d_fc_file_1800[i];
                d_f_current = _current_frequency;
                break;
            }
        }
    }
}
double
file_source_impl::arfcn2downlink(int arfcn) {
    double _frequency;
//    	std::cout<<"arfcn\t"<<std::cout<<arfcn<<std::endl;
    if(arfcn >= 0 && arfcn <=124)
        _frequency = (890 + 0.2*arfcn +45)*1e6;
    if(arfcn >= 512 && arfcn <= 885)
        _frequency = (1710.2 + 0.2*(arfcn - 512) + 95)*1e6;
    if(arfcn >= 975 && arfcn <=1023)
        _frequency = (890 + 0.2*(arfcn - 1024) +45)*1e6;
    return _frequency;
}
void
file_source_impl::handle_msg(pmt::pmt_t msg) {
    if(pmt::dict_has_key(msg, pmt::intern("arfcn"))) {
        pmt::pmt_t x = pmt::dict_ref(msg, pmt::intern("arfcn"), pmt::PMT_NIL );
        int _arfcn = pmt::to_uint64(x);
        if(_arfcn == d_arfcn[d_i]) {
            d_update_file = true;
        }

    }
}
uint64_t
file_source_impl::get_sample_offset() {
    if(d_arfcn[d_arfcn.size() - 2] <= 124)
        return round(((d_sample_offset + d_count_sample) / 6) * (1625000 * d_osr)/ d_sample_rate_out_900);
    if(d_arfcn[d_arfcn.size() - 2] >= 512)
        return round(((d_sample_offset + d_count_sample) / 6) * (1625000 *d_osr)/ d_sample_rate_out_1800);
}
void
file_source_impl::reset_file_source(std::vector<int> arfcn, std::vector<int> flag, uint64_t sample_offset){
    close();
    d_arfcn.clear();
    d_flag_arfcn.clear();
    copy(arfcn.begin(), arfcn.end(), back_inserter(d_arfcn));
    copy(flag.begin(), flag.end(), back_inserter(d_flag_arfcn));

      if(d_arfcn[0] >=512)
          d_sample_offset = d_sample_rate_out_1800 * sample_offset / (1625000 * d_osr / 6);
      if(d_arfcn[0] <=124)
          d_sample_offset = d_sample_rate_out_900 * sample_offset / (1625000 * d_osr / 6);
    int loop = d_sample_offset / 268435456;
    int _offset = d_sample_offset % 268435456;
    file_info.ID = 0;
    d_id = 0;
    d_update_file = false;
    d_res = 0;
    d_offset = 0;
    d_flag_start = true;
    d_i = 0;
    d_count_sample = 0;
    /*find file from arfcn*/
    find_fc_from_arfcn(d_arfcn[d_i]);
    for(int i = 0; i<= loop; i++)
        d_res = find_file(fileinfo, d_fc_file_current);
    if(d_res != -1)
        set_file(file_info.filepath.c_str(), _offset);
}

std::string
file_source_impl::get_table_name() {
    std::ifstream iFile ("/tmp/db_config");
    if (iFile.is_open())
    {
       std::ostringstream text;
        text << iFile.rdbuf();
        std::string file = text.str();
        file.erase(std::remove(file.begin(), file.end(), '\n'), file.end());

        return file;
    }
    std::cout<<"[ERROR] file_source_impl: Can not open file /tmp/db_config\n";
    return "";
}
uint64_t
file_source_impl::get_number_file_delete(double fc) {
      int number_file = 0;
      try {
    //         sql::Driver *mysql_driver;
    //         sql::Connection *mysql_con;
    //         sql::Statement *mysql_stmt;
          sql::ResultSet *res = NULL;
          char *zErrMsg = 0;
          char sql[1000] = {0};
          sprintf(sql, "SELECT COUNT(*) FROM `%s` WHERE deleted=1 AND status='Down' AND fc=%f AND index_round=%llu",d_table_name.c_str(), fc, d_index);
          res = mysql_stmt->executeQuery(sql);
          if (!res)
              return -1;
         while (res->next()) {
             std::string _number_file = res->getString(1);
             number_file = std::strtoull(_number_file.c_str(),NULL,0);
         }
          delete res;
       }
       catch (sql::SQLException &e) {
           std::cout << "ERROR: SQLException in " << __FILE__;
           std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
           std::cout << "ERROR: " << e.what();
           std::cout << " (MySQL error code: " << e.getErrorCode();
           std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
       }
       return number_file;
}
uint64_t 
file_source_impl::get_index() {
        uint64_t index = 0;
    try {
        sql::ResultSet *res = NULL;
        char *zErrMsg = 0;
        char sql[1000] = {0};
        sprintf(sql, "SELECT index_round FROM `%s` ORDER by ID DESC LIMIT 1 ",d_table_name.c_str());
        res = mysql_stmt->executeQuery(sql);
        while (res->next()) {
            index = std::strtoull(res->getString(1).c_str(),NULL, 0);
        }
        delete res;
        return index;
}
    catch (sql::SQLException &e) {
        std::cout << "ERROR: SQLException in " << __FILE__;
        std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
        std::cout << "ERROR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
        return 0;
    }
}
int
file_source_impl::l1_capture_info(uint64_t index) {
   try {
       sql::ResultSet *res = NULL;
       char *zErrMsg = 0;
       char sql[1000] = {0};
       double freq = 0;

       sprintf(sql, "SELECT DISTINCT fc, bwidth FROM `%s` WHERE index_round = %d",d_table_name.c_str(), index);
       res = mysql_stmt->executeQuery(sql);
       while (res->next()) {
           freq = (double)std::strtoull(res->getString(1).c_str(),NULL, 0);
           if(freq < 1800e6)
           {
             d_fc_file_900.push_back(freq);
             d_sample_rate_out_900 = (double)std::strtoull(res->getString(2).c_str(),NULL, 0);              
           }
           else
           {
             d_fc_file_1800.push_back(freq);
              d_sample_rate_out_1800 = (double)std::strtoull(res->getString(2).c_str(),NULL, 0);
           
           }
       }
       delete res;
       return 1;
   }
   catch (sql::SQLException &e) {
       std::cout << "ERROR: SQLException in " << __FILE__;
       std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
       std::cout << "ERROR: " << e.what();
       std::cout << " (MySQL error code: " << e.getErrorCode();
       std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
       return 0;
   }
}

uint64_t 
file_source_impl::get_real_offset() {
      uint64_t fc_900 = d_grd_config.get_fcapture_gsm() + 0.5e6;
      uint64_t fc_1800 = d_grd_config.get_fcapture_dcs();
      uint64_t real_offset_900 = std::numeric_limits<uint64_t>::max(); 
      uint64_t real_offset_1800 = std::numeric_limits<uint64_t>::max();
      if(fc_900 > 0)
       real_offset_900  = get_capture_offset(fc_900);
      if(fc_1800 > 0)
       real_offset_1800  = get_capture_offset(fc_1800);
      printf("DEBUG -------------- offset 900 %llu offset 1800 %llu\n", real_offset_900, real_offset_1800);

      return std::min(real_offset_900, real_offset_1800);
}

int
file_source_impl::find_file_start(uint64_t loop) {
      try {
      int ret = -1;
      int i = 0;
      sql::ResultSet *res = NULL;
      char *zErrMsg = 0;
      char sql[1000] = {0};
      sprintf(sql, "SELECT * FROM `%s` WHERE status='Down' AND fc=%f AND index_round=%llu ORDER BY ID", d_table_name.c_str(), d_fc_file_current, d_index);
      res = mysql_stmt->executeQuery(sql);
      if (!res)
          return -1;
      while (res->next()) {
        if(i == loop) {
            ret = 0;
            file_info.filepath =  res->getString(2);
            file_info.status =  res->getString(3);
            file_info.fc =  atof(res->getString(4).c_str());
            file_info.bwidth = atof(res->getString(5).c_str());
            file_info.ID = std::strtoull(res->getString(1).c_str(),NULL, 0);
        }
        i++;
      }
      delete res;
      return ret;
   }
   catch (sql::SQLException &e) {
       std::cout << "ERROR: SQLException in " << __FILE__;
       std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
       std::cout << "ERROR: " << e.what();
       std::cout << " (MySQL error code: " << e.getErrorCode();
       std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
       return -1;
   }

}
uint64_t
file_source_impl::get_current_offset(int ma) {
        if(ma <= 124)
            return round((d_sample_offset * 1625000 * d_osr) / (6 * d_sample_rate_out_900));
        if(ma >= 512)
            return round((d_sample_offset * 1625000 * d_osr) / (6 * d_sample_rate_out_1800));

}
uint64_t
file_source_impl::get_capture_offset(uint64_t fc) {
      try {
        sql::ResultSet *res = NULL;
        char *zErrMsg = 0;
        char sql[1000] = {0};
        sprintf(sql, "SELECT filepath, bwidth FROM `%s` WHERE status='Down' AND fc=%lu AND index_round=%llu", d_table_name.c_str(),fc, d_index);
        res = mysql_stmt->executeQuery(sql);
        if (!res)
            return -1;
        uint64_t number_rows = res->rowsCount();
        uint64_t sample = (number_rows - 1) * 268435456;
        res->last();
        double bwidth = atof(res->getString(2).c_str());
        std::string file_path = res->getString(1);
        std::ifstream in(file_path.c_str(), std::ifstream::ate | std::ifstream::binary);
        sample = sample + in.tellg() / 8;
        in.close();
        sample = sample * d_osr * 1625000 /(6 * bwidth);
        delete res;
        return sample;
      }
      catch (sql::SQLException &e) {
        std::cout << "ERROR: SQLException in " << __FILE__;
        std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
        std::cout << "ERROR: " << e.what();
        std::cout << " (MySQL error code: " << e.getErrorCode();
        std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
        return -2;
   }
}
} /* namespace blocks */
} /* namespace gr */

