/* -*- c++ -*- */
/*
 * @file
 * @author (C) 2015-2016 by Piotr Krysik <ptrkrysik@gmail.com>
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

#ifndef INCLUDED_GSM_CONFIG_H
#define INCLUDED_GSM_CONFIG_H

#include <grgsm/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace gsm {
    class GRGSM_API grd_config : virtual public gr::block
    {

    	/* Configure parameter that is used in grd.*/
     public:
      virtual float get_sampout_gsm()=0;
      virtual float get_sampout_dcs()=0;
      virtual float get_sampcap_gsm()=0;
      virtual float get_sampcap_dcs()=0;
      virtual double get_fcapture_dcs()=0;
      virtual double get_fcapture_gsm()=0;
      virtual std::string get_filepathup_gsm()=0;
      virtual std::string get_filepathdown_gsm()=0;
      virtual std::string get_filepathup_dcs()=0;
      virtual std::string get_filepathdown_dcs()=0;
      virtual std::string get_burst_folder()=0;
      virtual int get_osr()=0;
      virtual std::string get_database()=0;
      virtual std::string get_username()=0;
      virtual std::string get_password()=0;
      virtual int get_timescan()=0;
      virtual std::string get_table_name()=0;
      virtual int get_rotate_file()=0;
      virtual void set_fc_capture_900(double fc)=0;
      virtual void set_fc_capture_1800(double fc)=0;
      typedef boost::shared_ptr<grd_config> sptr;
      static sptr make();
    };

  } // namespace gsm
} // namespace gr

#endif

