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
#include "file_source_impl_C0.h"
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

file_source_C0::sptr file_source_C0::make(size_t itemsize,
		uint64_t offset, std::string f, bool uplink, uint64_t index) {
	return gnuradio::get_initial_sptr(
			new file_source_impl_C0(itemsize, offset, f, uplink, index));
}

file_source_impl_C0::file_source_impl_C0(size_t itemsize,
		uint64_t offset, std::string f, bool uplink, uint64_t index) :
		sync_block("file_source_C0", io_signature::make(0, 0, 0),
				io_signature::make(1, 1, itemsize)), d_itemsize(itemsize), d_fp(
				0), d_new_fp(0), d_updated(false), d_fc(f), d_uplink(uplink), d_index(index) {
	d_table_name = d_grd_config.get_table_name();
#ifdef SQLITE3
	  int rc = sqlite3_open("/etc/data/file.db", &fileinfo);

	  if( rc ) {
	      fprintf(stderr, "file_source_impl_C0:Can't open database: %s\n", sqlite3_errmsg(fileinfo));
	  } else {
	      fprintf(stderr, "file_source_impl_C0:Opened database successfully\n");
	   }
#else
  	  mysql_driver = get_driver_instance();
  	  mysql_con = mysql_driver->connect(d_grd_config.get_database(), d_grd_config.get_username(), d_grd_config.get_password());
  	  /* Connect to the MySQL test database */
  	  mysql_con->setSchema("file.db");
  	  mysql_stmt = mysql_con->createStatement();
#endif
	// d_index = get_index();
	int64_t loop = offset / 268435456;
	int _offset = offset % 268435456;
	file_info.ID = 0;
	d_id = 0;
	d_update_file = false;
	d_res = 0;
	if(loop < 0)
		loop = 0;
	for(int i = 0; i<= loop; i++)
		d_res = find_file(fileinfo);
	
	// if(d_res == -1) {
	// 	while(deadLine) {
	// 		d_res = find_file(fileinfo);
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
	message_port_register_out(pmt::mp("message_out"));
}

file_source_impl_C0::~file_source_impl_C0() {
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

bool file_source_impl_C0::seek(long seek_point, int whence) {
	return fseek((FILE*) d_fp, seek_point * d_itemsize, whence) == 0;
}

void file_source_impl_C0::open(const char *filename) {
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

void file_source_impl_C0::close() {
	// obtain exclusive access for duration of this function
	gr::thread::scoped_lock lock(fp_mutex);

	if (d_new_fp != NULL) {
		fclose(d_new_fp);
		d_new_fp = NULL;
	}
	d_updated = true;
}

void file_source_impl_C0::do_update() {
	if (d_updated) {
		gr::thread::scoped_lock lock(fp_mutex); // hold while in scope

		if (d_fp)
			fclose(d_fp);

		d_fp = d_new_fp;    // install new file pointer
		d_new_fp = 0;
		d_updated = false;
	}
}

int file_source_impl_C0::work(int noutput_items,
		gr_vector_const_void_star &input_items,
		gr_vector_void_star &output_items) {
	char *o = (char*) output_items[0];
	static unsigned char test[2];
	int i;
	int size = noutput_items;
	if(d_update_file) {
		d_update_file = false;
		close();
		int loop = d_sample_offset / 268435456;
		int _offset = d_sample_offset % 268435456;
		file_info.ID = 0;
		d_id = 0;
		for(int i = 0; i<= loop; i++)
			d_res = find_file(fileinfo);
		
		// if(d_res == -1) {
		// 	while(deadLine) {
		// 		d_res = find_file(fileinfo);
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
	}
	if(d_res) {
		std::cout<<"offset file is out of range"<<std::endl;
		return -1;
	}
	if(file_info.is_deleted == 1)
	{
		std::cout<<"offset belong to the file has been deleted"<<std::endl;
		pmt::pmt_t msg = pmt::make_tuple(pmt::mp("file_deleted"));
		//Todo: GET and SEND nearest offset avaiable to higher layer
		message_port_pub(pmt::mp("message_out"), msg);
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
		int deadLine = 20;
		int ret = -1; 
		while (deadLine) {
	    	deadLine --;
			ret = find_file(fileinfo);
			if (ret != -1)
				break;
				sleep(1);
		}
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

	return noutput_items;
}

void file_source_impl_C0::set_file(const char* file_name, uint64_t offset_byte) {
	d_flag_file = false;
	open(file_name);
	do_update();
	seek(offset_byte, SEEK_SET);
	}
#ifdef SQLITE3
int
file_source_impl_C0::callback(void *_fileinfo, int argc, char **argv, char **azColName) {
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
file_source_impl_C0::find_file(sqlite3* database){
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
	perror("file_source_impl_C0:Read database fail!!!");
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
 	  if(d_uplink) {
 		  sprintf(sql, "SELECT * FROM `%s` WHERE ID > %d AND status='Up' AND fc=%s AND index_round=%llu Limit 1",d_table_name.c_str(), file_info.ID, d_fc.c_str(), d_index);
 	  }
 	  else {
 		  sprintf(sql, "SELECT * FROM `%s` WHERE ID > %d AND status='Down' AND fc=%s AND index_round=%llu Limit 1",d_table_name.c_str(), file_info.ID, d_fc.c_str(),d_index);
 	  }
 	 res = mysql_stmt->executeQuery(sql);
 	 if (!res)
 		 return -1;
 	 while (res->next()) {
 		file_info.filepath =  res->getString(2);
 		file_info.status =  res->getString(3);
 		file_info.fc =  atof(res->getString(4).c_str());
 		file_info.bwidth = atof(res->getString(5).c_str());
 		file_info.ID = std::strtoull(res->getString(1).c_str(),NULL, 0);
		file_info.is_deleted = atoi(res->getString(8).c_str());
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

  return 0;
}

void
file_source_impl_C0::set_file_info(std::string fc, uint64_t sample_offset) {
	d_fc = fc;
	d_sample_offset = sample_offset;
	d_update_file = true;
}

std::string
file_source_impl_C0::get_fc_file() {
	return d_fc;
}

std::string
file_source_impl_C0::get_table_name() {
	std::ifstream iFile ("/tmp/db_config");
	if (iFile.is_open())
	{
	   std::ostringstream text;
	    text << iFile.rdbuf();
	    std::string file = text.str();
	    file.erase(std::remove(file.begin(), file.end(), '\n'), file.end());

	    return file;
	}
	std::cout<<"[ERROR] file_source_impl_C0: Can not open file /tmp/db_config\n";
	return "";
}

uint64_t
file_source_impl_C0::get_number_file_delete() {
	  int number_file = 0;
	  try {
	//         sql::Driver *mysql_driver;
	//         sql::Connection *mysql_con;
	//         sql::Statement *mysql_stmt;
	 	 sql::ResultSet *res = NULL;
	 	 char *zErrMsg = 0;
	 	 char sql[1000] = {0};
	 	  if(d_uplink) {
	 		  sprintf(sql, "SELECT COUNT(*) FROM `%s` WHERE deleted=1 AND status='Up' AND fc=%s AND index_round=%llu",d_table_name.c_str(), d_fc.c_str(), d_index);
	 	  }
	 	  else {
	 		  sprintf(sql, "SELECT COUNT(*) FROM `%s` WHERE deleted=1 AND status='Down' AND fc=%s AND index_round=%llu",d_table_name.c_str(), d_fc.c_str(), d_index);
	 	  }
	 	 res = mysql_stmt->executeQuery(sql);

	 	 if (!res)
	 		 return -1;
		 while (res->next()) {
	 		std::string _number_file = res->getString(1);
	 		number_file = std::strtoull(_number_file.c_str(),NULL,0);
	 		std::cout<<"[DEBUG] file_source_impl_C0 number_file \t"<<number_file<<std::endl;
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
file_source_impl_C0::get_index() {
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


   } /* namespace blocks */
 } /* namespace gr */
