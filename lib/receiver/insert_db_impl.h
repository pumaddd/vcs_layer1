/* -*- c++ -*- */
/* @file
 * @author Vadim Yanitskiy <axilirator@gmail.com>
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
 * 
 */

#ifndef INCLUDED_GRGSM_TRX_BURST_IF_IMPL_H
#define INCLUDED_GRGSM_TRX_BURST_IF_IMPL_H

#include <stddef.h>

#include <grgsm/gsmtap.h>
#include <grgsm/receiver/insert_db.h>
#include <sqlite3.h>
#include <iomanip>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "grd_config_impl.h"
#include <fstream>
#include <sstream>

namespace gr {
  namespace gsm {

    class insert_db_impl : public insert_db
    {
     private:
      std::string d_fc;
      std::string d_file_path;
      std::string d_date;
      float d_band_width;
      sql::Driver *mysql_driver;
      sql::Connection *mysql_con;
      sql::Statement *mysql_stmt;
      grd_config_impl d_grd_config;
      std::string d_status;
      boost::mutex d_mutex;
      bool detect_rach(uint8_t *burst);
      void burst_pack(pmt::pmt_t msg, uint8_t *buf);
      sqlite3 *sqlt;
      int create_new_table();
      std::string get_time_now();
      int d_count_file;
      std::string d_table_name;
      std::string d_operator_name;
      bool check_table_exist();
      void insert_table_name_to_db();
      int64_t d_index;
      int64_t get_index();
     public:
      insert_db_impl(std::string operator_name);
      ~insert_db_impl();
      static int callback(void *NotUsed, int argc, char **argv, char **azColName);
      void handle_msg(pmt::pmt_t msg);
      int insertDB(sqlite3* db, char* filepath, std::string fc, float bandwidth);
//      static int callback(void *NotUsed, int argc, char **argv, char **azColName);
    };

  } // namespace gsm
} // namespace gr

#endif /* INCLUDED_GRGSM_TRX_BURST_IF_IMPL_H */

