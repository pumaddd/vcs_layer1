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

#ifndef INCLUDED_BLOCKS_FILE_SOURCE_IMPL_C0_H
#define INCLUDED_BLOCKS_FILE_SOURCE_IMPL_C0_H

#include <grgsm/receiver/file_source_C0.h>
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
	  		   int id;
	  		   std::string filepath;
	  		   std::string status;
	  		   float fc;
	  		   float bwidth;
	  		   int ID;
           bool is_deleted;
} t_file_info;

namespace gr {
  namespace gsm {
    class GRGSM_API file_source_impl_C0 : public file_source_C0
    {
    private:
      size_t d_itemsize;
      FILE *d_fp;
      FILE *d_new_fp;
      bool d_repeat;
      bool d_updated;
      bool d_flag_file;
      uint64_t d_id;
      uint64_t d_sample_offset;
      bool d_update_file;
      boost::mutex fp_mutex;
      float d_f;
      void do_update();
      int find_db_via_id(sqlite3* database);
      int find_file(sqlite3* database);
      uint64_t get_number_file_delete();
      sqlite3* fileinfo;
      bool d_uplink;
      std::string d_fc;
      sql::Driver *mysql_driver;
      sql::Connection *mysql_con;
      sql::Statement *mysql_stmt;
      grd_config_impl d_grd_config;
      int d_res;
      uint64_t d_index;
      std::string d_table_name;
    public:
      //static int callback(void *NotUsed, int argc, char **argv, char **azColName);
      file_source_impl_C0(size_t itemsize, uint64_t offset, std::string f, bool uplink, uint64_t index);
      ~file_source_impl_C0();
  	  t_file_info file_info;
      void set_file(uint64_t offset_byte);
      bool seek(long seek_point, int whence);
      void open(const char *filename);
      uint64_t get_index();
      void close();
      static int callback(void *_fileinfo, int argc, char **argv, char **azColName);
      virtual void set_file(const char* file_name, uint64_t offset_byte);
	  virtual void set_file_info(std::string fc, uint64_t sample_offset);
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
      std::string get_table_name();
	  virtual std::string get_fc_file();
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_SOURCE_IMPL_H */
