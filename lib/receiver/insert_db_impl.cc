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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <boost/lexical_cast.hpp>

#include "grgsm/endian.h"
#include "grgsm/misc_utils/udp_socket.h"
#include "insert_db_impl.h"

/**
 * 41-bit RACH synchronization sequence
 * GSM 05.02 Chapter 5.2.7 Access burst (AB)
 */
namespace gr {
  namespace gsm {

  insert_db::sptr
  	  insert_db::make(std::string operator_name)
    {

      return gnuradio::get_initial_sptr
        (new insert_db_impl(operator_name));
    }

    /*
     * The private constructor
     */
  	  insert_db_impl::insert_db_impl(std::string operator_name
    ) : gr::block("insert_db_impl",
        gr::io_signature::make(0, 0, 0),
        gr::io_signature::make(0, 0, 0)),
		d_operator_name(operator_name)
    {
#ifdef SQLITE3
  		int ret = sqlite3_open("/etc/data/file.db", &sqlt);
  		if( ret ) {
 	      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(sqlt));
  		} else {
 	      fprintf(stderr, "Opened database successfully\n");
  		}
  		sqlite3_exec(sqlt, "pragma journal_mode = WAL", NULL, NULL, NULL);
#else
	  	  mysql_driver = get_driver_instance();
	  	  mysql_con = mysql_driver->connect(d_grd_config.get_database(), d_grd_config.get_username(), d_grd_config.get_password());
	  	  /* Connect to the MySQL test database */
	  	  mysql_con->setSchema("file.db");
	  	  mysql_stmt = mysql_con->createStatement();
	  	  d_table_name = d_grd_config.get_table_name();
	  	  create_new_table();
#endif
  		d_fc = "";
  		d_band_width = 0;
  		d_count_file = 0;
		d_index = get_index();
		d_index = d_index + 1;
        message_port_register_in(pmt::mp("msg_in"));
        // Bind a port handler
        set_msg_handler(pmt::mp("msg_in"),
        boost::bind(&insert_db_impl::handle_msg, this, _1));
    }

    /*
     * Our virtual destructor.
     */
  	insert_db_impl::~insert_db_impl()
    {
#ifdef SQLITE3
    	sqlite3_close(sqlt);
#else
    	mysql_con->close();
    	mysql_stmt->close();
    	delete mysql_con;
    	delete mysql_stmt;
#endif

    }

    void
	insert_db_impl::handle_msg(pmt::pmt_t msg)
    {
    	gr::thread::scoped_lock lock(d_mutex);
        if(pmt::is_dict(msg)) {
        	if(pmt::dict_has_key(msg, pmt::intern("freq_center"))) {
        		pmt::pmt_t x = pmt::dict_ref(msg, pmt::intern("freq_center"), pmt::PMT_NIL );
        		d_fc = pmt::symbol_to_string(x);
        	}
        	if(pmt::dict_has_key(msg, pmt::intern("file_path"))) {
        		pmt::pmt_t x = pmt::dict_ref(msg, pmt::intern("file_path"), pmt::PMT_NIL );
        		d_file_path = pmt::symbol_to_string(x);
        	}
        	if(pmt::dict_has_key(msg, pmt::intern("band_width"))) {
        		pmt::pmt_t x = pmt::dict_ref(msg, pmt::intern("band_width"), pmt::PMT_NIL );
        		d_band_width = pmt::to_float(x);
        	}
        	if(pmt::dict_has_key(msg, pmt::intern("status"))) {
        		pmt::pmt_t x = pmt::dict_ref(msg, pmt::intern("status"), pmt::PMT_NIL );
        		d_status = pmt::symbol_to_string(x);
        	}
        	if(pmt::dict_has_key(msg, pmt::intern("date"))) {
        		pmt::pmt_t x = pmt::dict_ref(msg, pmt::intern("date"), pmt::PMT_NIL );
        		d_date = pmt::symbol_to_string(x);
        	}
        }
        insertDB(sqlt, (char*)d_file_path.c_str(), d_fc, d_band_width);
    }
#ifdef SQLITE3
       int
	   insert_db_impl::callback(void *NotUsed, int argc, char **argv, char **azColName) {
       int i;
       printf("argc = %d", argc);
       for(i = 0; i<argc; i++) {
          printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
       }
       printf("\n");
       return 0;
    }
#endif
    int
    insert_db_impl::insertDB(sqlite3* db, char* filepath, std::string fc, float bandwidth){
#ifdef SQLITE3
       int rc;
       char *zErrMsg = 0;
       char sql[1000] = {0};
       sprintf(sql, "INSERT INTO \'%s\'(filepath, status, fc, bwidth, date, operator) VALUES (\"%s\", \"%s\", %s, %f, %s, %s)","FileInfo", filepath, d_status.c_str(), fc.c_str(), bandwidth, d_date.c_str(), d_operator_name.c_str());
       while(1){
       rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
          perror("insert_db_impl:Indert database fail!!!");
          usleep(1);
          sqlite3_free(zErrMsg);
        } else {
     	   break;
        }
       }
#else
       char sql[1000] = {0};
       try {
  //         sql::Driver *mysql_driver;
  //         sql::Connection *mysql_con;
  //         sql::Statement *mysql_stmt;
      	 sql::ResultSet *res = NULL;
      	 char *zErrMsg = 0;
      	 sql::PreparedStatement *pstmt;
      	 sprintf(sql, "INSERT INTO `%s`(filepath, status, fc, bwidth, date, operator, index_round) VALUES (\"%s\", \"%s\", %s, %f, %s, \"%s\", %llu)",d_table_name.c_str(), filepath, d_status.c_str(), fc.c_str(), bandwidth, d_date.c_str(),d_operator_name.c_str(), d_index);
		 pstmt = mysql_con->prepareStatement(sql);
      	 res = pstmt->executeQuery();
      	 pstmt->close();
      	 delete res;
      	 delete pstmt;
        }
        catch (sql::SQLException &e) {
          std::cout<<sql<<std::endl;
      	  std::cout << "ERROR: SQLException in " << __FILE__;
      	  std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
      	  std::cout << "ERROR: " << e.what();
      	  std::cout << " (MySQL error code: " << e.getErrorCode();
      	  std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
        }
#endif

       return 0;
     }
    std::string
	insert_db_impl::get_time_now(){
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
        std::string _time_now(buf);
        return _time_now;
    }
    int
	insert_db_impl::create_new_table() {
    	bool _check_table_exist =  check_table_exist();

    	if(!_check_table_exist)
    	 {
    		char sql[1000] = {0};
    		sprintf(sql, "CREATE TABLE `%s` (\
`ID` int(64) NOT NULL AUTO_INCREMENT,\
`filepath` text NOT NULL,\
`status` text NOT NULL,\
`fc` int(11) NOT NULL,\
`bwidth` int(11) DEFAULT NULL,\
`date` int(11) DEFAULT NULL,\
`flag` int(11) DEFAULT '0',\
`deleted` int(11) DEFAULT '0',\
`operator` text NOT NULL,\
`index_round` int(64) DEFAULT '0' ,\
PRIMARY KEY (`ID`)\
) ENGINE=InnoDB DEFAULT CHARSET=latin1",d_table_name.c_str());

    		try {
    			sql::ResultSet *res = NULL;
    			sql::PreparedStatement *pstmt;
    			pstmt = mysql_con->prepareStatement(sql);
    			res = pstmt->executeQuery();
    			pstmt->close();
    			if (!res) {
    				std::cout<<"[EROR] insert_db_impl: Can not create table on database\n";
    				delete res;
    				delete pstmt;
    				return -1;
    			}

    			delete res;
    			delete pstmt;
    		}
    		catch (sql::SQLException &e) {
    			std::cout << "ERROR: SQLException in " << __FILE__;
    			std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
    			std::cout << "ERROR: " << e.what();
    			std::cout << " (MySQL error code: " << e.getErrorCode();
    			std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
    		}
    		insert_table_name_to_db();
    	 }
       return 0;
    }

    bool
	insert_db_impl::check_table_exist()
    {
    	 char sql[1000] = {0};
    	 sprintf(sql, "SHOW TABLES LIKE \'%s\'", d_table_name.c_str());
    	 sql::ResultSet *res = NULL;
     	 sql::PreparedStatement *pstmt;
     	 pstmt = mysql_con->prepareStatement(sql);
     	 res = pstmt->executeQuery();
     	 pstmt->close();
		 while(res->next()) {
     	 	if(res->getString(1) != "") {
     			 delete res;
     		 	delete pstmt;
     		 	return true;
     		 }
     	 	else {
     		 	delete res;
     		 	delete pstmt;
     		 	return false;
     	 	}
		 }
    }
    void
	insert_db_impl::insert_table_name_to_db() {
        try {
   //         sql::Driver *mysql_driver;
   //         sql::Connection *mysql_con;
   //         sql::Statement *mysql_stmt;
       	 sql::ResultSet *res = NULL;
       	 char *zErrMsg = 0;
       	 char sql[1000] = {0};
       	 sql::PreparedStatement *pstmt;
       	 std::string time_now = get_time_now();
       	 sprintf(sql, "INSERT INTO `ManageFile`(tablename, date) VALUES (\"%s\", \"%s\")",d_table_name.c_str(), time_now.c_str());
		 std::cout<<sql<<std::endl;
       	 pstmt = mysql_con->prepareStatement(sql);
       	 res = pstmt->executeQuery();
       	 pstmt->close();
       	 delete res;
       	 delete pstmt;
         }
         catch (sql::SQLException &e) {
       	  std::cout << "ERROR: SQLException in " << __FILE__;
       	  std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
       	  std::cout << "ERROR: " << e.what();
       	  std::cout << " (MySQL error code: " << e.getErrorCode();
       	  std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
         }
    }
int64_t 
insert_db_impl::get_index() {
		uint64_t index = -1;
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
		return -1;
	}
}



  } /* namespace gsm */
} /* namespace gr */
