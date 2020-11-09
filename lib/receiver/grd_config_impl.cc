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
#include "grd_config_impl.h"

/**
 * 41-bit RACH synchronization sequence
 * GSM 05.02 Chapter 5.2.7 Access burst (AB)
 */
namespace gr {
  namespace gsm {

  grd_config::sptr
  	  grd_config::make()
    {

      return gnuradio::get_initial_sptr
        (new grd_config_impl());
    }

    /*
     * The private constructor
     */
  	  grd_config_impl::grd_config_impl(
    ) : gr::block("grd_config_impl",
        gr::io_signature::make(0, 0, 0),
        gr::io_signature::make(0, 0, 0))
    {
  		  read_config_file();
    }
    /*
     * Our virtual destructor.
     */
  	grd_config_impl::~grd_config_impl()
    {
    }
	void
	grd_config_impl::read_config_file() {
		std::ifstream cFile ("/etc/data/grd_config");
		if (cFile.is_open())
		{
		   std::string line;
		   while(getline(cFile, line)){
			     std::istringstream is_line(line);
				 std::string key;
				 if( std::getline(is_line, key, '=') )
				  {
					if(key=="SAMPRATEOUTGSM900") {
				    std::string value;
				    std::getline(is_line, value);
				    d_samprateout_gsm = std::strtof(value.c_str(), NULL);
					}
					if(key=="SAMPRATEOUTDCS1800") {
				    std::string value;
				    std::getline(is_line, value);
				    d_samprateout_dcs = std::strtof(value.c_str(), NULL);
					}
					if(key=="SAMPCAPDCS1800") {
				    std::string value;
				    std::getline(is_line, value);
				    d_sampcap_dcs = std::strtof(value.c_str(), NULL);
					}
					if(key=="SAMPCAPGSM900") {
				    std::string value;
				    std::getline(is_line, value);
				    d_sampcap_gsm = std::strtof(value.c_str(), NULL);
					}
					if(key=="FCAPTUREGSM900") {
				    std::string value;
				    std::getline(is_line, value);
				    d_fcapture_gsm = std::strtold(value.c_str(), NULL);
				    s_fcapture_gsm = value;
					}
					if(key=="FCAPTUREDCS1800") {
				    std::string value;
				    std::getline(is_line, value);
				    d_fcapture_dcs = std::strtold(value.c_str(), NULL);
				    s_fcapture_dcs = value;
					}
					if(key=="FILEPATHUPGSM900") {
				    std::string value;
				    std::getline(is_line, value);
				    filepathup_gsm = value;
					}
					if(key=="FILEPATHDOWNGSM900") {
				    std::string value;
				    std::getline(is_line, value);
				    filepathdown_gsm = value;
					}
					if(key=="FILEPATHUPDCS1800") {
				    std::string value;
				    std::getline(is_line, value);
				    filepathup_dcs = value;
					}
					if(key=="FILEPATHDOWNDCS1800") {
				    std::string value;
				    std::getline(is_line, value);
				    filepathdown_dcs = value;
					}
					if(key=="BURSTFOLDER") {
				    std::string value;
				    std::getline(is_line, value);
				    d_burst_folder = value;
					}
					if(key=="OVERSAMPRATE") {
				    std::string value;
				    std::getline(is_line, value);
				    d_osr = std::strtoull(value.c_str(),NULL,0);
					}
					if(key=="DATABASE") {
				    std::string value;
				    std::getline(is_line, value);
				    database = value;
					}
					if(key=="USERNAME") {
				    std::string value;
				    std::getline(is_line, value);
				    username = value;
					}
					if(key=="PASSWORD") {
				    std::string value;
				    std::getline(is_line, value);
				    password = value;
					}
					if(key=="TIMESCAN") {
				    std::string value;
				    std::getline(is_line, value);
				    d_time_scan =  std::strtoull(value.c_str(), NULL,0);
					}
					if(key=="TABLENAME") {
				    std::string value;
				    std::getline(is_line, value);
				    d_table_name = value;
					}
					if(key=="ROTATEFILE") {
				    std::string value;
				    std::getline(is_line, value);
				    d_rotate_file =  std::strtoull(value.c_str(), NULL,0);
					}
				  }
		    }
		}
		    else {
		        std::cerr << "Couldn't open config file for reading.\n";
		    }
	}
	float
	grd_config_impl::get_sampout_gsm() {
		return d_samprateout_gsm;
	}
	float
	grd_config_impl::get_sampout_dcs() {
		return d_samprateout_dcs;
	}
	float
	grd_config_impl::get_sampcap_gsm() {
		return d_sampcap_gsm;
	}
	float
	grd_config_impl::get_sampcap_dcs() {
		return d_sampcap_dcs;
	}
	double
	grd_config_impl::get_fcapture_dcs() {
		return d_fcapture_dcs;
	}
	double
	grd_config_impl::get_fcapture_gsm() {
		return d_fcapture_gsm;
	}
	std::string
	grd_config_impl::get_filepathup_gsm() {
		return filepathup_gsm;
	}
	std::string
	grd_config_impl::get_filepathdown_gsm() {
		return filepathdown_gsm;
	}
	std::string
	grd_config_impl::get_filepathup_dcs() {
		return filepathup_dcs;
	}
	std::string
	grd_config_impl::get_filepathdown_dcs() {
		return filepathdown_dcs;
	}
	int
	grd_config_impl::get_osr() {
		return d_osr;
	}
    std::string
	grd_config_impl::get_database() {
    	return database;
    }
    std::string
	grd_config_impl::get_username() {
    	return username;
    }
    std::string
	grd_config_impl::get_password() {
    	return password;
    }
    int
	grd_config_impl::get_timescan() {
    	return d_time_scan;
    }
    std::string
	grd_config_impl::get_table_name() {
    	return d_table_name;
    }
    std::string
	grd_config_impl::get_burst_folder() {
    	return d_burst_folder;
    }
    int
	grd_config_impl::get_rotate_file() {
    	return d_rotate_file;
    }
    void grd_config_impl::set_fc_capture_1800(double fc) {
		std::ifstream iFile ("/etc/data/grd_config");
		if (iFile.is_open())
		{
		   std::ostringstream text;
		    text << iFile.rdbuf();
		    std::string file = text.str();
		    std::ostringstream strs;
		    strs<<fc;
		    std::string replace = strs.str();
		    replace.erase(std::remove(replace.begin(), replace.end(), '+'), replace.end());
		    size_t pos = file.find("FCAPTUREDCS1800=");
		    file.replace(pos + 16, s_fcapture_dcs.length(), replace);
		    iFile.close();
		    std::ofstream out_file("/etc/data/grd_config");
		    out_file << file;
		    out_file.close();
		}
    }

    void grd_config_impl::set_fc_capture_900(double fc) {
		std::ifstream iFile ("/etc/data/grd_config");
		if (iFile.is_open())
		{
		   std::ostringstream text;
		    text << iFile.rdbuf();
		    std::string file = text.str();
		    std::ostringstream strs;
		    strs<<fc;
		    std::string replace = strs.str();
		    replace.erase(std::remove(replace.begin(), replace.end(), '+'), replace.end());
		    size_t pos = file.find("FCAPTUREGSM900=");
		    file.replace(pos + 15, s_fcapture_gsm.length(), replace);
		    iFile.close();
		    std::ofstream out_file("/etc/data/grd_config");
		    out_file << file;
		    out_file.close();
		}
    }

  }
}
