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
#ifndef INCLUDED_GSM_DECODEC0_IMPL_H
#define INCLUDED_GSM_DECODEC0_IMPL_H
#include <grgsm/receiver/decodeC0.h>
#include <grgsm/gsmtap.h>
#include <grgsm/gsm_constants.h>
#include <receiver_config.h>
#include <vector>
#include <sqlite3.h>
#include "time_sample_ref.h"
#include "grd_config_impl.h"
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>
#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/blocks/rotator.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/fft_filter.h>
#include <gnuradio/filter/fft_filter_ccf.h>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
using namespace gr::filter;
typedef struct database{
	uint8_t name_event;
	uint64_t sample_offset;
	int t1, t2, t3;
	double  ppm;
	int bsic;
} data_insert_to_db;
namespace gr {
  namespace gsm {
    class decodeC0_impl : public decodeC0
    {
     private:
        float d_mu;
        long double d_mu_inc;
        std::vector<data_insert_to_db> data_insert;
        // int d_nsamples;
        blocks::rotator d_r;
        unsigned int d_samples_consumed;
        bool d_rx_time_received;
        time_sample_ref d_time_samp_ref;
        int d_c0_burst_start;
        float d_c0_signal_dbm;
        int d_input_process;
        grd_config_impl d_grd_config;

        uint64_t d_index;
        /**@name Configuration of the receiver */
        //@{
        const int d_OSR; ///< oversampling ratio
        bool d_process_uplink;
        const int d_chan_imp_length; ///< channel impulse length
        float d_signal_dbm;
        std::vector<int> d_tseq_nums; ///< stores training sequence numbers for channels different than C0
        std::vector<int> d_cell_allocation;
        long double d_fc;
        long double d_fcO;
        boost::mutex fp_mutex;
        long double d_sample_rate;///< stores cell allocation - absolute rf channel numbers (ARFCNs) assigned to the given cell. The variable should at least contain C0 channel number.
        //@}
        gr_complex d_sch_training_seq[N_SYNC_BITS]; ///<encoded training sequence of a SCH burst
        gr_complex d_norm_training_seq[TRAIN_SEQ_NUM][N_TRAIN_BITS]; ///<encoded training sequences of a normal and dummy burst
        float d_last_time;
        std::string d_table_name;
        sqlite3 *dbsqlt;
        long double d_start;
        uint64_t d_sample_offset;
        long double SAMPLERATE_OUT;

        /** Counts samples consumed by the receiver
         *
         * It is used in beetween find_fcch_burst and reach_sch_burst calls.
         * My intention was to synchronize this counter with some internal sample
         * counter of the USRP. Simple access to such USRP's counter isn't possible
         * so this variable isn't used in the "synchronized" state of the receiver yet.
         */
        unsigned d_counter;
        /**@name Variables used to store result of the find_fcch_burst fuction */
        //@{
        bool d_freq_offset_tag_in_fcch; ///< frequency offset tag presence
        unsigned d_fcch_start_pos; ///< position of the first sample of the fcch burst
        float d_freq_offset_setting; ///< frequency offset set in frequency shifter located upstream
        //@}
        std::list<double> d_freq_offset_vals;
        /**@name Identifiers of the BTS extracted from the SCH burst */
        //@{
        int d_ncc; ///< network color code
        int d_bcc; ///< base station color code
        //@}
        /**@name Internal state of the gsm receiver */
        //@{
        enum states {
          fcch_search, sch_search, // synchronization search part
          synchronized // receiver is synchronized in this state
        } d_state;
        //@}
        /**@name Variables which make internal state in the "synchronized" state */
        //@{
        burst_counter d_burst_nr; ///< frame number and timeslot number
        channel_configuration d_channel_conf; ///< mapping of burst_counter to burst_type
        //@}
        unsigned int d_sample_counter;
        unsigned int d_normal_burst;
        unsigned int d_fch_burst;
        unsigned int d_sch_burst;
        unsigned int d_dummy_burst;
        unsigned int d_exit_channel_counter;
        int d_cell_id; 
        int d_lac_id;
        unsigned d_failed_sch; ///< number of subsequent erroneous SCH bursts
        int d_states;
        sql::Driver *mysql_driver;
        sql::Connection *mysql_con;
        sql::Statement *mysql_stmt;
        float d_fraction_sample;

        /** Function whis is used to search a FCCH burst and to compute frequency offset before
        * "synchronized" state of the receiver
        *
        * @param input vector with input signal
        * @param nitems number of samples in the input vector
        * @return
        */
        bool find_fcch_burst(const gr_complex *input, const int nitems, double & computed_freq_offset);
        /** Computes frequency offset from FCCH burst samples
         *
         * @param[in] input vector with input samples
         * @param[in] first_sample number of the first sample of the FCCH busrt
         * @param[in] last_sample number of the last sample of the FCCH busrt
         * @param[out] computed_freq_offset contains frequency offset estimate if FCCH burst was located
         * @return true if frequency offset was faound
         */

        double compute_freq_offset(const gr_complex * input, unsigned first_sample, unsigned last_sample);
        /** Computes angle between two complex numbers
         *
         * @param val1 first complex number
         * @param val2 second complex number
         * @return
         */
        inline float compute_phase_diff(gr_complex val1, gr_complex val2);
        /** Function whis is used to get near to SCH burst
         *
         * @param nitems number of samples in the gsm_receiver's buffer
         * @return true if SCH burst is near, false otherwise
         */
        bool reach_sch_burst(const int nitems);
        /** Extracts channel impulse response from a SCH burst and computes first sample number of this burst
         *
         * @param input vector with input samples
         * @param chan_imp_resp complex vector where channel impulse response will be stored
         * @return number of first sample of the burst
         */
        int get_sch_chan_imp_resp(const gr_complex *input, gr_complex * chan_imp_resp);
        /** MLSE detection of a burst bits
         *
         * Detects bits of burst using viterbi algorithm.
         * @param input vector with input samples
         * @param chan_imp_resp vector with the channel impulse response
         * @param burst_start number of the first sample of the burst
         * @param output_binary vector with output bits
         */
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
        void send_burst(burst_counter burst_nr, const unsigned char * burst_binary, uint8_t burst_type, size_t input_nr, unsigned int burst_start=-1, unsigned int snr = 0);
        float d_phase_inc;
        /**
         * Configures burst types in different channels
         */
        void configure_receiver();
        /* State machine handlers */
        void fcch_search_handler(gr_complex *input, int noutput_items);
        void sch_search_handler(gr_complex *input, int noutput_items);
        void synchronized_handler(gr_complex *input, int noutput_items);
        inline uint64_t resample(const gr_complex *in,
             uint64_t first_in_sample,
             gr_complex *out,
             uint64_t first_out_sample,
             uint64_t samples_to_produce);
        int find_ppm(sqlite3* database);
     public:
        decodeC0_impl(int osr, const std::vector<int> &cell_allocation, double fc, float fcO, double sample_rate,
        			  uint64_t sample_offset, std::string table_name, uint64_t index, bool process_uplink);
        ~decodeC0_impl();
        void forecast(int noutput_items,
  		       gr_vector_int &ninput_items_required);
        int general_work(int noutput_items,
  		       gr_vector_int &ninput_items,
  		       gr_vector_const_void_star &input_items,
  		       gr_vector_void_star &output_items);
        virtual void set_cell_allocation(const std::vector<int> &cell_allocation);
        virtual void set_tseq_nums(const std::vector<int> & tseq_nums);
        virtual void reset();
        gr_complex calc_phase_offset_consume(const gr_complex phase_inc,const gr_complex phase_offset, int num_sample);
        int insertDB(uint8_t nameEvent, uint64_t sampleOffset, int t1, int t2, int t3, double ppm, int bsic);
        static int callback(void *NotUsed, int argc, char **argv, char **azColName);
        virtual void set_fc(float fc);
        virtual void set_fc0(float fcO);
        virtual void set_sample_rate(float sample_rate);
        virtual void set_cell_lac(int cell_id, int lac_id);

    };
  } // namespace gsm
} // namespace gr
#endif /* INCLUDED_GSM_DECODEC0_IMPL_H */
