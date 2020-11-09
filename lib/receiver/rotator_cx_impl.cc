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
#include <stdlib.h>

#include <boost/circular_buffer.hpp>
#include <boost/scoped_ptr.hpp>
#include <grgsm/endian.h>

#include "rotator_cx_impl.h"
#include "viterbi_detector.h"
#include "sch.h"

#if 0
/* Files included for debuging */
#include "plotting/plotting.hpp"
#include <pthread.h>
#include <iomanip>
#endif

#define SYNC_SEARCH_RANGE 30

namespace gr
{
  namespace gsm
  {
    /* The public constructor */
  	  rotator_cx::sptr
	  rotator_cx::make(
			const std::vector<int> &cell_allocation, const std::vector<double> &fc, float bd,
			double sample_rate, std::string table_name, uint64_t id_start, double samp_in, uint64_t index, bool uplink)
    {
      return gnuradio::get_initial_sptr
        (new rotator_cx_impl(cell_allocation, fc, bd, sample_rate, table_name, id_start, samp_in, index, uplink));
    }

    /* The private constructor */
  	rotator_cx_impl::rotator_cx_impl(
  			const std::vector<int> &cell_allocation, const std::vector<double> &fc, float bd,
			double sample_rate, std::string table_name, uint64_t id_start, double samp_in, uint64_t index, bool uplink
    ) : gr::block("hopping_frequency",
        gr::io_signature::make(1, -1, sizeof(gr_complex)),
        gr::io_signature::make(1, -1, sizeof(gr_complex))),
		d_ma(cell_allocation),
        d_burst_nr(0),
		d_burst_nr_uplink(0),
		d_fc(fc),
		d_bd(bd),
    	d_sample_rate(sample_rate),
  		d_mai(0),
  		d_table_name(table_name),
  		d_id_start(id_start),
		d_samp_in(samp_in),
		d_uplink(uplink),
		d_index(index)
    {

  	   /**
       * Configure the receiver,
       * i.e. tell it where to find which burst type
       */
  		SAMPLERATE_OUT = (1625000.0/6.0 * d_grd_config.get_osr());
        d_mu_inc = (double)(d_samp_in / SAMPLERATE_OUT);
        d_narfcn = d_ma.size();
        d_input_nr = d_fc.size();
        d_sample_per_burst = floor(156.25 * d_grd_config.get_osr() * d_mu_inc);
        d_mu = (float)d_samp_in / (float)d_sample_rate;
        d_frame_number = 0;
        d_i = 0;
        d_ppm = 0;
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
	  	mysql_con = mysql_driver->connect(d_grd_config.get_database(), d_grd_config.get_username(), d_grd_config.get_password());
	  	/* Connect to the MySQL test database */
	  	mysql_con->setSchema("test.db");
	  	mysql_stmt = mysql_con->createStatement();
#endif
        find_db_via_id(database, d_id_start);
        d_sample = d_sample_start = db_01.sample_offset;
//        std::cout<<d_sample<<"\t"<<d_sample_start<<std::endl;
        set_output_multiple(d_sample_per_burst);
    }

    /* Our virtual destructor */
  	rotator_cx_impl::~rotator_cx_impl() {
#ifdef SQLITE3
    	sqlite3_close(database);
#else
    	mysql_con->close();
    	mysql_stmt->close();
    	delete mysql_con;
    	delete mysql_stmt;
#endif
  	}

    void
	rotator_cx_impl::forecast(int noutput_items,
                            gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++) {
        ninput_items_required[i] = 2 * noutput_items;
      }

    }

    int
	rotator_cx_impl::general_work(int noutput_items,
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
        int process_in_offset = 0;
        uint64_t a = floor(d_sample_start);
        if (start <  a)
        {
      	  if ((a - start) >= 2307) {
      	      consume_each (2307);
      		  return 2307;
      	  }
      	  else {
      		  consume_each((a - start));
      		  return (a - start);
      	  }
        }
        a = ceil(db_01.sample_offset);
        if ((a - start) < d_sample_per_burst && (a - start) >= 0) {
      	  if(db_01.name_event == 1) {
      		  	  d_ppm = db_01.ppm_01;
      		  	  find_db_via_id(database, (db_01.id + 1));
      	  }
        }
        a = ceil(db_01.sample_offset);
        if ((a - start) <  d_sample_per_burst && (a - start) >= 0) {
        	  if(db_01.name_event == 2) {
     			 find_db_via_id(database, (db_01.id + 1));
        	  }
        }

        out = (gr_complex *) output_items[0];
        double frequency_hopper = arfcn2downlink(d_ma[0]);
        in = (gr_complex *) input_items[0];
        double phase_inc = -2 * M_PI * (frequency_hopper - d_fc[0]) / d_samp_in
      				  	  	+ d_ppm/1.0e6*2*M_PI*(frequency_hopper) / d_samp_in;
        d_r.set_phase_incr(exp(gr_complex(0 , phase_inc)));
        d_r.rotateN(out, in, d_sample_per_burst + process_in_offset);

        if(d_uplink) {
        	out_uplink = (gr_complex *) output_items[1];
        	double frequency_hopper = arfcn2downlink(d_ma[0]);
        	in_uplink = (gr_complex *) input_items[1];
        	double phase_inc = -2 * M_PI * (frequency_hopper - d_fc[0]) / d_samp_in
      				  	  	  	+ d_ppm/1.0e6*2*M_PI*(frequency_hopper - 95e6)/ d_samp_in;
        	d_r_uplink.set_phase_incr(exp(gr_complex(0 , phase_inc)));
        	d_r_uplink.rotateN(out_uplink, in_uplink, d_sample_per_burst + process_in_offset);
        }
        consume_each(d_sample_per_burst + process_in_offset);
        return (d_sample_per_burst + process_in_offset);
    }
#ifdef SQLITE3
 	int
	rotator_cx_impl::callback(void *_db_01, int argc, char **argv, char **azColName) {
 		 int i;
 		 param_01* db_01 = (param_01*) _db_01;
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
     	   db_01->ppm_01 = std::atof(argv[i]);
       }

    }
    return 0;
  }
#endif
    int
	rotator_cx_impl::find_db_via_id(sqlite3* database, int ID){
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
     		db_01.ppm_01 = atof(res->getString(7).c_str());
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
//    int
//	rotator_cx_impl::calculate_ma_sfh(int maio, int hsn, int n, int fn)
//    {
//        int mai = 0;
//        int s = 0;
//        int nbin = floor(log2(n) + 1);
//        int t1 = fn / 1326;
//        int t2 = fn % 26;
//        int t3 = fn % 51;
//
//        if (hsn == 0)
//            mai = (fn + maio) % n;
//        else {
//            int t1r = t1 % 64;
//            int m = t2 + RNTABLE_06[(hsn ^ t1r) + t3];
//            int mprim = m % (1 << nbin);
//            int tprim = t3 % (1 << nbin);
//
//            if (mprim < n)
//                s = mprim;
//            else
//                s = (mprim + tprim) % n;
//
//            mai = (s + maio) % n;
//        }
//
//        return (mai);
//    }
    double
	rotator_cx_impl::arfcn2downlink(int arfcn) {
    	double _frequency;
    	if(arfcn >= 0 && arfcn <=124)
    		_frequency = (890 + 0.2*arfcn +45)*1e6;
    	if(arfcn >= 512 && arfcn <= 885)
    		_frequency = (1710.2 + 0.2*(arfcn - 512) + 95)*1e6;
    	if(arfcn >= 975 && arfcn <=1023)
    		_frequency = (890 + 0.2*(arfcn - 1024) +45)*1e6;
    	return _frequency;
    }

    void
	rotator_cx_impl::set_ma(std::vector<int> ma) {
    	d_ma.clear();
    	d_ma = ma;
    }
    void
	rotator_cx_impl::set_fc(std::vector<double> fc) {
    	d_fc.clear();
    	d_fc = fc;
    }
    void
	rotator_cx_impl::set_id_start(int id_start) {
    	d_id_start = id_start;
        find_db_via_id(database, d_id_start);
        d_sample = d_sample_start = db_01.sample_offset;
    }
    void
	rotator_cx_impl::set_sample_start(int sample_start) {
    	d_sample = sample_start;
    }
    void
	rotator_cx_impl::set_sample_rate(float set_sample_rate) {
    	d_sample_rate = set_sample_rate;
    }
    void
	rotator_cx_impl::set_bandwidth(float bandwidth) {
    	d_bd = bandwidth;
    }
  } /* namespace gsm */
} /* namespace gr */
