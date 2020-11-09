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

#ifndef INCLUDED_GSM_SUPER_BLOCK_IMPL_H
#define INCLUDED_GSM_SUPER_BLOCK_IMPL_H

#include <grgsm/receiver/super_block.h>
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
	float ppm;
} param;
namespace gr {
  namespace gsm {

    class super_block_impl : public super_block
    {
     private:
        long double d_mu_inc;
        float d_ppm;
        unsigned int d_samples_consumed;
        bool d_rx_time_received;
        time_sample_ref d_time_samp_ref;
        const int d_OSR; ///< oversampling ratio
        const int d_chan_imp_length; ///< channel impulse length
        float d_signal_dbm;
        std::vector<int> d_tseq_nums; ///< stores training sequence numbers for channels different than C0
        std::vector<int> d_ma;
        param db;
        int d_maio;
        int d_hsn;
        int d_tseq;
        long double d_fcO;
        uint64_t d_id_start;
        uint64_t d_sample;
        long double d_sample_rate;
        sqlite3* database;
        uint64_t d_sample_start;
        float d_sample_fractional;
        int d_mai;
        int d_narfcn;
        int d_burst_start;
        long double d_sample_per_burst;
        bool d_flag_decoder;
        burst_type d_btype;
        std::string d_table_name;
        gr_complex d_norm_training_seq[TRAIN_SEQ_NUM][N_TRAIN_BITS]; ///<encoded traical_numinput_by_numoutput(uint64_t samples_to_produce, float &d_acc, unsigned int &d_last_filter)ning sequences of a normal and dummy burst
        uint32_t d_frame_number;
        burst_counter d_burst_nr;
        long double SAMPLERATE_OUT;
        ///< frame number and timeslot number
        channel_configuration d_channel_conf; ///< mapping of burst_counter to burst_type
        std::string d_fc;
        sql::Driver *mysql_driver;//      		std::cout<<d_ppm<<"\t"<<db_01.ppm<<"\t"<<db_01.sample_offset<<std::endl;

        sql::Connection *mysql_con;
        sql::Statement *mysql_stmt;
        grd_config_impl d_grd_config;
        uint64_t d_index;

        void detect_burst(const gr_complex * input, gr_complex * chan_imp_resp, int burst_start, unsigned char * output_binary);

        /** Encodes differentially input bits and maps them into MSK states
         *
         * @param input vector with input bits
         * @param nitems number of samples in the "input" vector
         * @param gmsk_output bits mapped into MSK states
         * @param start_point first state
         */
        void gmsk_mapper(const unsigned char * input, int nitems, gr_complex * gmsk_output, gr_complex start_point);

        /** Correlates MSK mapped sequence with input signal
         *
         * @param sequence MKS mapped sequence
         * @param length length of the sequence
         * @param input_signal vector with input samples
         * @return correlation value
         */
        gr_complex correlate_sequence(const gr_complex * sequence, int length, const gr_complex * input);

        /** Computes autocorrelation of input vector for positive arguments
         *
         * @param input vector with input samples
         * @param out output vector
         * @param nitems length of the input vector
         */
        inline void autocorrelation(const gr_complex * input, gr_complex * out, int nitems);

        /** Filters input signal through channel impulse response
         *
         * @param input vector with input samples
         * @param nitems number of samples to pass through filter
         * @param filter filter taps - channel impulse response
         * @param filter_length nember of filter taps
         * @param output vector with filtered samples
         */
        inline void mafi(const gr_complex * input, int nitems, gr_complex * filter, int filter_length, gr_complex * output);

        /**  Extracts channel impulse response from a normal burst and computes first sample number of this burst
         *
         * @param input vector with input samples
         * @param chan_imp_resp complex vector where channel impulse response will be stored
         * @param search_range possible absolute offset of a channel impulse response start
         * @param bcc base station color code - number of a training sequence
         * @return first sample number of normal burst
         */
        int get_norm_chan_imp_resp(const gr_complex *input, gr_complex * chan_imp_resp, float *corr_max, int bcc, double &valid);

        /**
         * Sends burst through a C0 (for burst from C0 channel) or Cx (for other bursts) message port
         *
         * @param burst_nr - frame number of the burst
         * @param burst_binary - content of the burst
         * @b_type - type of the burst
         */
        void send_burst(burst_counter burst_nr, const unsigned char * burst_binary, uint8_t burst_type, size_t input_nr,
        				unsigned int burst_start=-1, unsigned int snr=0, bool uplink=true);
        float d_phase_inc;
        /**
         * Configures burst types in different channels
         */
        void configure_receiver();

        /* State machine handlers */
        void synchronized_handler(gr_complex *input, int noutput_items, bool uplink, int i);

        int calculate_ma_sfh(int maio, int hsn, int n, int fn);
     public:
        super_block_impl(int osr, const std::vector<int> &cell_allocation, int maio, int hsn,
        				 int tseq, double sample_rate, std::string table_name, uint64_t id_start, uint64_t index);
        ~super_block_impl();
        void forecast(int noutput_items,
  		       gr_vector_int &ninput_items_required);
        int general_work(int noutput_items,
  		       gr_vector_int &ninput_items,
  		       gr_vector_const_void_star &input_items,
  		       gr_vector_void_star &output_items);
        int find_db_via_id(sqlite3* db, int id);
        virtual void set_ma(std::vector<int> ma);
        virtual void set_maio(int maio);
        virtual void set_hsn(int hsn);
        virtual void set_id_start(int id_start);
        virtual void set_sample_start(int sample_start);
        virtual void set_sample_rate(float set_sample_rate);
        virtual void set_tseq(int tseq);
        static int callback(void *_db, int argc, char **argv, char **azColName);


    };
  } // namespace gsm
} // namespace gr

#endif /* INCLUDED_GSM_SUPER_BLOCK_IMPL_H */

