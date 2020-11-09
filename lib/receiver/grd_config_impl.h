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

#ifndef INCLUDED_BLOCKS_GRD_CONFIG_IMPL_H
#define INCLUDED_BLOCKS_GRD_CONFIG_IMPL_H

#include <grgsm/receiver/grd_config.h>
#include <boost/thread/mutex.hpp>
#include <fstream>
#include <sstream>
#include "grd_config_impl.h"

namespace gr {
  namespace gsm {
    class GRGSM_API grd_config_impl : public grd_config
    {
    private:
    	void read_config_file();
    	float d_samprateout_gsm;
    	float d_samprateout_dcs;
    	float d_sampcap_gsm;
    	float d_sampcap_dcs;
    	double d_fcapture_gsm;
    	double d_fcapture_dcs;
    	int d_time_scan;
    	std::string d_table_name;
    	int d_rotate_file;
    	std::string filepathup_gsm;
    	std::string filepathdown_gsm;
    	std::string filepathup_dcs;
    	std::string filepathdown_dcs;
    	std::string database;
    	std::string username;
    	std::string password;
    	std::string s_fcapture_dcs;
    	std::string s_fcapture_gsm;
    	std::string d_burst_folder;
    	int d_osr;
    public:
      grd_config_impl();
      ~grd_config_impl();
      virtual float get_sampout_gsm();
      virtual float get_sampout_dcs();
      virtual float get_sampcap_gsm();
      virtual float get_sampcap_dcs();
      virtual double get_fcapture_dcs();
      virtual double get_fcapture_gsm();
      virtual int get_timescan();
      virtual std::string get_filepathup_gsm();
      virtual std::string get_filepathdown_gsm();
      virtual std::string get_filepathup_dcs();
      virtual std::string get_filepathdown_dcs();
      virtual std::string get_burst_folder();
      virtual int get_osr();
      virtual std::string get_database();
      virtual std::string get_username();
      virtual std::string get_password();
      virtual std::string get_table_name();
      virtual int get_rotate_file();
      virtual void set_fc_capture_900(double fc);
      virtual void set_fc_capture_1800(double fc);

    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_GRD_CONFIG_IMPL_H */
