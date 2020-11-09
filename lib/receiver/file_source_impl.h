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

#ifndef INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H
#define INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H

#include <grgsm/receiver/file_source.h>
#include <boost/thread/mutex.hpp>
#include <sqlite3.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "grd_config_impl.h"

typedef struct {
	  		   std::string filepath;
	  		   std::string status;
	  		   float fc;
	  		   float bwidth;
	  		   int ID;
} t_file_info;

namespace gr {
  namespace gsm {
    class GRGSM_API file_source_impl : public file_source
    {
    private:
      size_t d_itemsize;
      FILE *d_fp;
      FILE *d_new_fp;
      bool d_repeat;
      bool d_updated;
      bool d_flag_file;
      uint64_t d_id;
      int d_time_scan;
      uint64_t d_sample_offset;
      bool d_update_file;
      boost::mutex fp_mutex;
      float d_f;
      void do_update();
      int find_db_via_id(sqlite3* database);
      void handle_msg(pmt::pmt_t msg);
      void find_fc_from_arfcn(int arfcn);
      double arfcn2downlink(int arfcn);
      int find_file(sqlite3* database, double fc);
      uint64_t get_number_file_delete(double fc);
      sqlite3* fileinfo;
      bool d_uplink;
      std::vector<double> d_fc_file_900;
      std::vector<double> d_fc_file_1800;
      double d_fc_900;
      double d_fc_1800;
      double d_sample_rate_out_900;
      double d_sample_rate_out_1800;
      double d_sample_rate_cap_900;
      double d_sample_rate_cap_1800;
      double d_f_current;
      int d_osr;
      bool d_flag_start;
      double d_fc_file_current;
      std::vector<int> d_arfcn;
      std::vector<int> d_flag_arfcn;
      uint64_t d_offset;
      sql::Driver *mysql_driver;
      sql::Connection *mysql_con;
      sql::Statement *mysql_stmt;
      grd_config_impl d_grd_config;
      int d_res;
      int d_i;
      uint64_t d_count_sample;
      std::string d_table_name;
      uint64_t get_index();
      int l1_capture_info(uint64_t index);
      uint64_t d_index; 
    public:
      //static int callback(void *NotUsed, int argc, char **argv, char **azColName);
      file_source_impl(size_t itemsize, uint64_t offset, std::vector<int> arfcn, std::vector<int> flag, uint64_t index);
      ~file_source_impl();
  	  t_file_info file_info;
      void set_file(uint64_t offset_byte);
      bool seek(long seek_point, int whence);
      void open(const char *filename);
      void close();
      virtual uint64_t get_real_offset();
      uint64_t get_capture_offset(uint64_t fc);
      int find_file_start(uint64_t loop);
      uint64_t get_current_offset(int ma);
      uint64_t d_real_offset;
      static int callback(void *_fileinfo, int argc, char **argv, char **azColName);
      virtual void set_file(const char* file_name, uint64_t offset_byte);
      virtual uint64_t get_sample_offset();
//	    virtual void set_file_info(std::string fc, uint64_t sample_offset);
      virtual void reset_file_source(std::vector<int> arfcn, std::vector<int> flag, uint64_t sample_offset);
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
//      virtual std::string get_fc_file();
      std::string get_table_name();

    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H */
