/* -*- c++ -*- */
/*
 * @file
 * @author (C) 2009-2017 by Piotr Krysik <ptrkrysik@gmail.com>
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

#ifndef INCLUDED_GSM_ROTATOR_CX_IMPL_H
#define INCLUDED_GSM_ROTATOR_CX_IMPL_H

#include <grgsm/receiver/rotator_cx.h>
#include <grgsm/gsmtap.h>
#include <grgsm/gsm_constants.h>
#include <receiver_config.h>
#include <vector>
#include "time_sample_ref.h"
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>
#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/blocks/rotator.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/fir_filter.h>
#include <sqlite3.h>
#include "grd_config_impl.h"
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace gr::filter;
typedef struct database{
	uint64_t id;
	uint8_t name_event;
	uint64_t sample_offset;
	int t1, t2, t3;
	double ppm_01;
} param_01;
namespace gr {
  namespace gsm {
    class rotator_cx_impl : public rotator_cx
    {
     private:
    	param_01 db_01;
        blocks::rotator d_r;
        blocks::rotator d_r_uplink;
        double d_ppm;
        std::vector<int> d_ma;
        std:: vector<double> d_fc;
        int d_input_nr;
        int d_fn;
        float d_bd;
        float d_fcO;
        double d_sample_rate;
        sqlite3* database;
        uint64_t d_sample_start;
        int d_mai;
        int d_narfcn;
        int d_sample_per_burst;
        std::string d_table_name;
        double d_mu_inc;
        uint32_t d_frame_number;
        burst_counter d_burst_nr;
        burst_counter d_burst_nr_uplink;
        uint64_t d_id_start;
        double d_sample;
        int d_i;
        double d_samp_in;
        double d_mu;
        double d_offset;
        int d_noutput;
        bool d_uplink;
        sql::Driver *mysql_driver;
        sql::Connection *mysql_con;
        sql::Statement *mysql_stmt;
        grd_config_impl d_grd_config;
        uint64_t d_index;
        float SAMPLERATE_OUT;
        int calculate_ma_sfh(int maio, int hsn, int n, int fn);
     public:
        rotator_cx_impl(const std::vector<int> &cell_allocation, const std::vector<double> &fc, float bd,
        				double sample_rate, std::string table_name, uint64_t id_start, double samp_in, uint64_t index, bool uplink);
        ~rotator_cx_impl();
        void forecast(int noutput_items,
  		       gr_vector_int &ninput_items_required);
        int general_work(int noutput_items,
  		       gr_vector_int &ninput_items,
  		       gr_vector_const_void_star &input_items,
  		       gr_vector_void_star &output_items);
        int find_db_via_id(sqlite3* db, int id);
        double arfcn2downlink(int arfcn);
        virtual void set_ma(std::vector<int> ma);
        virtual void set_fc(std::vector<double> fc);
        virtual void set_id_start(int id_start);
        virtual void set_sample_start(int sample_start);
        virtual void set_sample_rate(float set_sample_rate);
        virtual void set_bandwidth(float bandwidth);
        static int callback(void *NotUsed, int argc, char **argv, char **azColName);
    };
  } // namespace gsm
} // namespace gr

#endif /* INCLUDED_GSM_SUPER_BLOCK_IMPL_H */
