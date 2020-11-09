/* -*- c++ -*- */
/*
 * @file
 * @author (C) 2009-2017  by Piotr Krysik <ptrkrysik@gmail.com>
 * @section LICENSE
 *
 * Gr-gsm is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Gr-gsm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gr-gsm; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>

#include <algorithm>
#include <string.h>
#include <iostream>
#include <numeric>
#include <vector>
#include <stdio.h>

#include <boost/circular_buffer.hpp>
#include <boost/scoped_ptr.hpp>
#include <grgsm/endian.h>

#include "hopping_frequency_impl.h"
#include "viterbi_detector.h"
#include "sch.h"

#if 0
/* Files included for debuging */
#include "plotting/plotting.hpp"
#include <pthread.h>
#include <iomanip>
#endif

#define SYNC_SEARCH_RANGE 30
//#define SAMPLERATE_OUT (1625000.0/6.0 * 2)

namespace gr
{
  namespace gsm
  {
    /* The public constructor */
  	hopping_frequency::sptr
	hopping_frequency::make(
			const std::vector<int> &cell_allocation, const std::vector<double> &fc, int maio,
			int hsn, float bd, double sample_rate, std::string table_name, uint64_t id_start, double samp_in, uint8_t time_slot, uint64_t index)
    {
      return gnuradio::get_initial_sptr
        (new hopping_frequency_impl(cell_allocation, fc, maio, hsn, bd, sample_rate, table_name, id_start, samp_in, time_slot, index));
    }

    /* The private constructor */
  	hopping_frequency_impl::hopping_frequency_impl(
  			const std::vector<int> &cell_allocation, const std::vector<double> &fc, int maio,
  			int hsn, float bd, double sample_rate, std::string table_name, uint64_t id_start, double samp_in, uint8_t time_slot, uint64_t index
    ) : gr::block("hopping_frequency",
        gr::io_signature::make(1, -1, sizeof(gr_complex)),
        gr::io_signature::make(2, 2, sizeof(gr_complex))),
		d_ma(cell_allocation),
        d_burst_nr(grd_config_impl().get_osr()),
		d_burst_nr_uplink(grd_config_impl().get_osr()),
		d_fc(fc),
		d_maio(maio),
		d_hsn(hsn),
		d_bd(bd),
    	d_sample_rate(sample_rate),
  		d_mai(0),
  		d_table_name(table_name),
  		d_id_start(id_start),
		d_samp_in(samp_in),
		d_time_slot(time_slot),
  		d_index(index)
    {

      /**
       * Configure the receiver,
       * i.e. tell it where to find which burst type
       */
  	  SAMPLERATE_OUT = (float)(1625000.0/6.0 * grd_config_impl().get_osr());
      d_mu_inc = (float)(d_samp_in / SAMPLERATE_OUT);
	  d_narfcn = d_ma.size();
	  d_input_nr = d_fc.size();
      d_sample_per_burst = ceil(156.25 * grd_config_impl().get_osr() * d_mu_inc);
      d_mu = (float)d_samp_in / (float)d_sample_rate;
      d_frame_number = 0;
      d_i = 0;
      d_ppm = 0;
	  process_in_offset = 0;
      
#ifdef SQLITE3
	  int rc = sqlite3_open("/etc/data/test.db", &database);
	   if( rc ) {
	      fprintf(stderr, "hopping_frequency_impl:Can't open database: %s\n", sqlite3_errmsg(database));
	   } else {
	      fprintf(stderr, "hopping_frequency_impl:Opened database successfully\n");
	   }
	   sqlite3_exec(database, "pragma journal_mode = WAL", NULL, NULL, NULL);
#else
	  	  mysql_driver = get_driver_instance();
	  	  mysql_con = mysql_driver->connect(grd_config_impl().get_database(), grd_config_impl().get_username(), grd_config_impl().get_password());
	  	  /* Connect to the MySQL test database */
	  	  mysql_con->setSchema("test.db");
	  	  mysql_stmt = mysql_con->createStatement();
#endif
       find_db_via_id(database, d_id_start);
       d_sample = d_sample_start = db_01.sample_offset;
       set_output_multiple(d_sample_per_burst);
    }

    /* Our virtual destructor */
  	hopping_frequency_impl::~hopping_frequency_impl() {
#ifdef SQLITE3
    	sqlite3_close(database);
#else
    	mysql_con->close();
    	mysql_stmt->close();
    	delete mysql_con;
    	delete mysql_stmt;
#endif

  	}

  	unsigned char RNTABLE_02[114] = {
	            48, 98, 63, 1, 36, 95, 78, 102, 94, 73, \
	            0, 64, 25, 81, 76, 59, 124, 23, 104, 100, \
	            101, 47, 118, 85, 18, 56, 96, 86, 54, 2, \
	            80, 34, 127, 13, 6, 89, 57, 103, 12, 74, \
	            55, 111, 75, 38, 109, 71, 112, 29, 11, 88, \
	            87, 19, 3, 68, 110, 26, 33, 31, 8, 45, \
	            82, 58, 40, 107, 32, 5, 106, 92, 62, 67, \
	            77, 108, 122, 37, 60, 66, 121, 42, 51, 126, \
	            117, 114, 4, 90, 43, 52, 53, 113, 120, 72, \
	            16, 49, 7, 79, 119, 61, 22, 84, 9, 97, \
	            91, 15, 21, 24, 46, 39, 93, 105, 65, 70, \
	            125, 99, 17, 123 \
	  };
    void
	hopping_frequency_impl::forecast(int noutput_items,
                            gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++) {
        ninput_items_required[i] = 2 * noutput_items;
      }

    }

    int
	hopping_frequency_impl::general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
        gr_complex *in;
        gr_complex *in_uplink;
        gr_complex *out = (gr_complex *) output_items[0];
        gr_complex *out_uplink = (gr_complex *) output_items[1];
        uint64_t start = nitems_read(0) + ceil(d_sample);
  	    int frame_number = 0;
        uint64_t a = floor(d_sample_start);
        if (start <  a)
        {
      	  if ((a - start) >= 1000) {
      	      consume_each(1000);
      		  return 1000;
      	  }
      	  else {
      		  consume_each((a - start));
      		  return (a - start);
      	  }
        }
        a = ceil(db_01.sample_offset);
        if ((a - start) <= d_sample_per_burst && (a - start) >= 0) {
      	  if(db_01.name_event == 1) {
      		  	  d_ppm = db_01.ppm;
      		  	  find_db_via_id(database, (db_01.id + 1));
      	  }
        }
        a = ceil(db_01.sample_offset);
        if ((a - start) <  d_sample_per_burst && (a - start) >= 0) {
        	  if(db_01.name_event == 2) {
        		  if((a - start) <= d_sample_per_burst/2) {
        			  d_burst_nr.set(db_01.t1, db_01.t2, db_01.t3, 0);
        			  d_burst_nr_uplink.set_uplink(db_01.t1, db_01.t2, db_01.t3, 0);
        			  process_in_offset = (a - start);
        		  } else {
    				  d_burst_nr.set(db_01.t1, db_01.t2, db_01.t3, 0);
    				  d_burst_nr_uplink.set_uplink(db_01.t1, db_01.t2, db_01.t3, 0);
    				  d_burst_nr--;
    				  d_burst_nr_uplink--;
        			  process_in_offset = -d_sample_per_burst + (a - start);
        		  }
     			 find_db_via_id(database, (db_01.id + 1));
        	  }
        }
		burst_counter l_burst_nr = d_burst_nr.subtract_timeslots(0);
        burst_counter l_burst_nr_ul = d_burst_nr_uplink.subtract_timeslots(0);
        if(d_time_slot == 5)
        {
        	l_burst_nr++;
			l_burst_nr_ul++;
        }
		if(d_time_slot == 4)
        {
        	l_burst_nr++;
			l_burst_nr++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
        }
        if(d_time_slot == 3)
        {
        	l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
        	l_burst_nr_ul++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
        }
        if(d_time_slot == 2)
        {
        	l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
        }
        if(d_time_slot == 1)
        {
        	l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
        	l_burst_nr_ul++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
        }
        if(d_time_slot == 0)
        {
        	l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
			l_burst_nr++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;
			l_burst_nr_ul++;

        }

        frame_number = l_burst_nr.get_frame_nr();
        d_mai = calculate_ma_sfh(d_maio, d_hsn, d_narfcn, frame_number);
        for(; d_i < d_input_nr; d_i++)
        {
      	  double frequency_hopper = arfcn2downlink(d_ma[d_mai]);
      	  if (frequency_hopper > (d_fc[d_i] - d_bd/2) && frequency_hopper < (d_fc[d_i] + d_bd/2)) {
      		  in = (gr_complex *) input_items[d_i];
      		  double phase_inc;
      		  phase_inc = -2 * M_PI * (frequency_hopper - d_fc[d_i]) / d_samp_in
      				  	  + d_ppm/1.0e6*2*M_PI*(frequency_hopper) / d_samp_in;
      		  d_r.set_phase_incr(exp(gr_complex(0 , phase_inc)));
      	  }
        }
		frame_number = l_burst_nr_ul.get_frame_nr();
		d_mai = calculate_ma_sfh(d_maio, d_hsn, d_narfcn, frame_number);
        for(; d_i < 2 * d_input_nr; d_i++)
        {
      	  double frequency_hopper = arfcn2downlink(d_ma[d_mai]);
      	  if (frequency_hopper > (d_fc[d_i - d_input_nr] - d_bd/2) && frequency_hopper < (d_fc[d_i - d_input_nr] + d_bd/2)) {
      		  in_uplink = (gr_complex *) input_items[d_i];
      		  double phase_inc;
      		  if(d_ma[0] >= 512)
      			  phase_inc = -2 * M_PI * (frequency_hopper - d_fc[d_i - d_input_nr]) / d_samp_in
      				  	  	  + d_ppm/1.0e6*2*M_PI*(frequency_hopper - 95e6)/ d_samp_in;
      		  else
      			phase_inc = -2 * M_PI * (frequency_hopper - d_fc[d_i - d_input_nr]) / d_samp_in
      			      				  	  	  + d_ppm/1.0e6*2*M_PI*(frequency_hopper - 45e6)/ d_samp_in;

      		    d_r_uplink.set_phase_incr(exp(gr_complex(0 , phase_inc)));
      	  }
        }
        d_r.rotateN(out, in, d_sample_per_burst);
        d_r_uplink.rotateN(out_uplink, in_uplink, d_sample_per_burst);
        d_burst_nr++;
        d_burst_nr_uplink++;
        d_i = 0;
        consume_each(d_sample_per_burst);
        return (d_sample_per_burst);
    }
#ifdef SQLITE3
 	int
	hopping_frequency_impl::callback(void *_db_01, int argc, char **argv, char **azColName) {
 		 int i;
 		 param* db_01 = (param*) _db_01;
 		 for(i = 0; i<argc; i++) {
 			 if((strcmp(azColName[i], "NameEvent")) == 0) {
 				 db_01->name_event = argv[i];
     	  //printf("DEBUG: Name event: %s \n", db.name_event.c_str());
 			 }
 			 if((strcmp(azColName[i],"ID")) == 0) {
 				 db_01->id = std::strtoull(argv[i],NULL,0);
 			 }
 			 if((strcmp(azColName[i],"SampleOffset")) == 0) {
     	   db_01->sample_offset = std::strtoull(argv[i],NULL,0);
     	   //std::cout<<"sample_offset: "<<db.sample_offset<<std::endl;
       }
       if((strcmp(azColName[i], "T1")) == 0) {
     	   db_01->t1 = atoi(argv[i]);
       }
       if((strcmp(azColName[i], "T2")) == 0) {
     	   db_01->t2 = atoi(argv[i]);
       }
       if((strcmp(azColName[i], "T3")) == 0) {
     	   db_01->t3 = atoi(argv[i]);
       }
       if((strcmp(azColName[i], "PPM")) == 0) {
     	   db_01->ppm = atof(argv[i]);
//     	   std::cout<<db_01->ppm<<"\t"<<argv[i]<<std::endl;
       }

    }
    return 0;
  }
#endif
    int
	hopping_frequency_impl::find_db_via_id(sqlite3* database, int ID){
#ifdef SQLITE3
      int rc;
      char *zErrMsg = 0;
      char sql[1000] = {0};
      sprintf(sql, "SELECT * FROM \'%s\' WHERE ID = %d", d_table_name.c_str(), ID);
      while(1){
      rc = sqlite3_exec(database, sql, callback, (void*)&this->db_01, &zErrMsg);
      if( rc != SQLITE_OK ) {
    	usleep(1);
    	perror("super_block_impl: Read database fail");
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
     	 sprintf(sql, "SELECT * FROM `%s` WHERE ID = %llu and Index_round = %llu", d_table_name.c_str(), ID, d_index);
     	 res = mysql_stmt->executeQuery(sql);
     	 while (res->next()) {
     		db_01.id = std::strtoull(res->getString(1).c_str(),NULL,0);
     		db_01.name_event = atoi(res->getString(2).c_str());
     		db_01.sample_offset = std::strtoull(res->getString(3).c_str(),NULL,0);
     		db_01.t1 = atoi(res->getString(4).c_str());
     		db_01.t2 = atoi(res->getString(5).c_str());
     		db_01.t3 = atoi(res->getString(6).c_str());
     		db_01.ppm = atof(res->getString(7).c_str());
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
      return 0;
    }
    int
	hopping_frequency_impl::calculate_ma_sfh(int maio, int hsn, int n, int fn)
    {
        int mai = 0;
        int s = 0;
        int nbin = floor(log2(n) + 1);
        int t1 = fn / 1326;
        int t2 = fn % 26;
        int t3 = fn % 51;

        if (hsn == 0)
            mai = (fn + maio) % n;
        else {
            int t1r = t1 % 64;
            int m = t2 + RNTABLE_02[(hsn ^ t1r) + t3];
            int mprim = m % (1 << nbin);
            int tprim = t3 % (1 << nbin);

            if (mprim < n)
                s = mprim;
            else
                s = (mprim + tprim) % n;

            mai = (s + maio) % n;
        }

        return (mai);
    }
    double
	hopping_frequency_impl::arfcn2downlink(int arfcn) {
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
	hopping_frequency_impl::set_ma(std::vector<int> ma) {
    	d_ma.clear();
    	d_ma = ma;
    }
    void
	hopping_frequency_impl::set_fc(std::vector<double> fc) {
    	d_fc.clear();
    	d_fc = fc;
    }
    void
	hopping_frequency_impl::set_maio(int maio) {
    	d_maio = maio;
    }
    void
	hopping_frequency_impl::set_hsn(int hsn) {
    	d_hsn = hsn;
    }
    void
	hopping_frequency_impl::set_id_start(int id_start) {
    	d_id_start = id_start;
        find_db_via_id(database, d_id_start);
        d_sample = d_sample_start = db_01.sample_offset;
    }
    void
	hopping_frequency_impl::set_sample_start(int sample_start) {
    	d_sample = sample_start;
    }
    void
	hopping_frequency_impl::set_sample_rate(float set_sample_rate) {
    	d_sample_rate = set_sample_rate;
    }
    void
	hopping_frequency_impl::set_bandwidth(float bandwidth) {
    	d_bd = bandwidth;
    }
  } /* namespace gsm */
} /* namespace gr */
