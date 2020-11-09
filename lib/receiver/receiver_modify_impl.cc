/* -*- c++ -*- */
/*
 * @file
 * @author (C) 2009-2017  by Piotr Krysik <ptrkrysik@gmail.com>
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <numeric>
#include <vector>
#include <boost/circular_buffer.hpp>
#include <boost/scoped_ptr.hpp>
#include <grgsm/endian.h>
#include "receiver_modify_impl.h"
#include "viterbi_detector.h"
#include "sch.h"
#if 0
/* Files included for debuging */
#include "plotting/plotting.hpp"
#include <pthread.h>
#include <iomanip>
#endif
#define SYNC_SEARCH_RANGE 30
#define SAMPLERATE_OUT (1625000.0/6.0 * 4)
namespace gr
{
  namespace gsm
  {
    /* The public constructor */
    receiver_modify::sptr
    receiver_modify::make(
      int osr, const std::vector<int> &cell_allocation, float fc, float fcO, float sample_rate, bool process_uplink)
    {
      return gnuradio::get_initial_sptr
        (new receiver_modify_impl(osr, cell_allocation, fc, fcO, sample_rate, process_uplink));
    }
    /* The private constructor */
    receiver_modify_impl::receiver_modify_impl(
      int osr, const std::vector<int> &cell_allocation, float fc, float fcO, float sample_rate, bool process_uplink
    ) : gr::block("receiver_modify",
          gr::io_signature::make(1, -1, sizeof(gr_complex)),
          gr::io_signature::make(1, 1, sizeof(gr_complex))),
        d_samples_consumed(0),
        d_rx_time_received(false),
        d_time_samp_ref(GSM_SYMBOL_RATE * osr),
        d_OSR(osr),
        d_process_uplink(process_uplink),
        d_chan_imp_length(CHAN_IMP_RESP_LENGTH),
        d_counter(0),       //TODO: use nitems_read instead of d_counter
        d_fcch_start_pos(0),
		d_cell_allocation(cell_allocation),
        d_freq_offset_setting(0),
        d_state(fcch_search),
        d_burst_nr(osr),
        d_failed_sch(0),
        d_signal_dbm(-120),
        d_last_time(0.0),
		d_fc(fc),
		d_fcO(fcO),
    	d_sample_rate(sample_rate)
    {
      /**
       * Don't send samples to the receiver
       * until there are at least samples for one
       */
      set_output_multiple(floor((TS_BITS + 2 * GUARD_PERIOD) * d_OSR));
      /**
       * Prepare SCH sequence bits
       *
       * (TS_BITS + 2 * GUARD_PERIOD)
       * Burst and two guard periods
       * (one guard period is an arbitrary overlap)
       */
      gmsk_mapper(SYNC_BITS, N_SYNC_BITS,
        d_sch_training_seq, gr_complex(0.0, -1.0));
      /* Prepare bits of training sequences */
      for (int i = 0; i < TRAIN_SEQ_NUM; i++) {
        /**float phase_inc = -2 * M_PI * (freq_offset_tmp - d_freq_offset_setting)/ d_sample_rate;
         * If first bit of the sequence is 0
         * => first symbol is 1, else -1
         */
        gr_complex startpoint = train_seq[i][0] == 0 ?
          gr_complex(1.0, 0.0) : gr_complex(-1.0, 0.0);
        gmsk_mapper(train_seq[i], N_TRAIN_BITS,
          d_norm_training_seq[i], startpoint);
      }
      /* Register output ports */
      message_port_register_out(pmt::mp("C0"));
      message_port_register_out(pmt::mp("CX"));
      message_port_register_out(pmt::mp("measurements"));
      /**
       * Configure the receiver,
       * i.e. tell it where to find which burst type
       */
      configure_receiver();
      d_mu = 0;
      d_mu_inc = d_sample_rate / SAMPLERATE_OUT;
      d_resamp = new mmse_fir_interpolator_cc();
      d_taps = gr::filter::firdes::low_pass(1, (1625000.0/6.0 * 4) , 125.0e3, 5.0e3, gr::filter::firdes::WIN_HAMMING,6.76);//(1625000.0/6.0 * 4)
	  d_filter = new gr::filter::kernel::fir_filter_ccf(1 , d_taps);
	  d_phase_inc = -2 * M_PI * (d_fcO - d_fc) / d_sample_rate;
	  d_r.set_phase_incr(exp(gr_complex(0, d_phase_inc)));
	  set_relative_rate(1.0 / d_mu_inc);
    }
    /* Our virtual destructor */
    receiver_modify_impl::~receiver_modify_impl() {}
    void
    receiver_modify_impl::forecast(int noutput_items,
                            gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++) {
        ninput_items_required[i] =
          (int)ceil((noutput_items * d_mu_inc * 2) + d_resamp->ntaps());
      }
    }
    gr_complex receiver_modify_impl::calc_phase_offset_consume(const gr_complex phase_inc,const gr_complex phase_offset, int num_sample)
    {
    	//
    	gr_complex oo;
    	oo = phase_offset;
		for(int i=0;i<num_sample;i++)
		{
			oo *= phase_inc;
			oo /=std::abs(oo);
		}
		return oo;
    }
    int
    receiver_modify_impl::general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
      gr_complex *in = (gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      gr_complex input[8192 * 4];
      gr_complex out1[8192 * 4];
      gr_complex out2[8192 * 4];
      gr_complex out3[8192 * 4];
      uint64_t start = nitems_read(0);
      uint64_t stop = start + noutput_items;
      d_freq_offset_tag_in_fcch = false;
      memset(out1,0, sizeof(gr_complex) * 8192 * 4);
      memset(out2,0, sizeof(gr_complex) * 8192 * 4);
      memset(out3,0, sizeof(gr_complex) * 8192 * 4);
      int nitems = round(noutput_items* d_mu_inc);
      gr_complex bak_phase_offset = d_r.d_phase;
      d_r.rotateN(out1, in, ninput_items[0]);
      int input_process = resample(out1, 0, out2, 0, noutput_items*2);
	  d_filter->filterN(input, out2 , noutput_items*2 );
#if 0
      /* FIXME: jak zrobić to rzutowanie poprawnie */
      std::vector<const gr_complex *> iii =
        (std::vector<const gr_complex *>) input_items;
#endif
      {
      /* Main state machine */
		  d_samples_consumed = 0;
		  switch (d_state) {
		  case fcch_search:
			fcch_search_handler(input , noutput_items);
			break;
		  case sch_search:
			sch_search_handler(input , noutput_items);
			break;
		  case synchronized:
			synchronized_handler(input, noutput_items);
			break;
		  }
      }
      memcpy(out,input, sizeof(gr_complex)* d_samples_consumed );
      float d_mu_bak = d_mu;
      input_process = cal_numinput_by_numoutput(0, 0, d_samples_consumed, d_mu_bak);
      d_mu  = d_mu_bak;
      gr_complex test = calc_phase_offset_consume(d_r.d_phase_incr, bak_phase_offset , input_process);
      d_r.d_phase = test;
      consume_each(input_process);
      return d_samples_consumed;
    }
    void
    receiver_modify_impl::fcch_search_handler(gr_complex *input, int noutput_items)
    {
      double freq_offset_tmp;
      if (!find_fcch_burst(input, noutput_items, freq_offset_tmp))
        return;
      d_freq_offset_setting += freq_offset_tmp;
      float phase_inc = -2 * M_PI * d_freq_offset_setting / d_sample_rate;
      d_r.set_phase_incr(exp(gr_complex(0, (d_phase_inc + phase_inc))));
      d_state = sch_search;
    }
    void
    receiver_modify_impl::sch_search_handler(gr_complex *input, int noutput_items)
    {
      std::vector<gr_complex> channel_imp_resp(CHAN_IMP_RESP_LENGTH * d_OSR);
      unsigned char burst_buf[BURST_SIZE];
      int rc, t1, t2, t3;
      int burst_start;
      /* Wait until we get a SCH burst */
      if (!reach_sch_burst(noutput_items))
        return;
      /* Get channel impulse response from it */
      burst_start = get_sch_chan_imp_resp(input, &channel_imp_resp[0]);
      /* Detect bits using MLSE detection */
      detect_burst(input, &channel_imp_resp[0], burst_start, burst_buf);
      /* Attempt to decode BSIC and frame number */
      rc = decode_sch(&burst_buf[3], &t1, &t2, &t3, &d_ncc, &d_bcc);
      if (rc) {
        /**
         * There is error in the SCH burst,
         * go back to the FCCH search state
         */
        d_state = fcch_search;
        return;
      }
      /* Set counter of bursts value */
      d_burst_nr.set(t1, t2, t3, 0);
      d_burst_nr++;
      /* Consume samples up to the next guard period */
      unsigned int to_consume = burst_start + BURST_SIZE * d_OSR + 4 * d_OSR;
      d_samples_consumed += to_consume;
      /* Update current state */
      d_state = synchronized;
    }
    void
    receiver_modify_impl::synchronized_handler(gr_complex *input, int noutput_items)
    {
      /**
       * In this state receiver is synchronized and it processes
       * bursts according to burst type for given burst number
       */
      std::vector<gr_complex> channel_imp_resp(CHAN_IMP_RESP_LENGTH * d_OSR);
      size_t inputs_to_process = d_cell_allocation.size();
      unsigned char output_binary[BURST_SIZE];
      burst_type b_type;
      int to_consume = 0;
      int offset = 0;
      /* Process all connected inputs */
     // for (size_t input_nr = 0; input_nr < inputs_to_process; input_nr++)
      {
    	 size_t input_nr = 0;
        //input = (gr_complex *) input_items[input_nr];
        double signal_pwr = 0;
        for (int ii = GUARD_PERIOD; ii < TS_BITS; ii++)
          signal_pwr += abs(input[ii]) * abs(input[ii]);
        signal_pwr = signal_pwr / (TS_BITS);
        d_signal_dbm = round(10 * log10(signal_pwr / 50));
        if (input_nr == 0)
          d_c0_signal_dbm = d_signal_dbm;
        /* Get burst type for given burst number */
        b_type = input_nr == 0 ?
          d_channel_conf.get_burst_type(d_burst_nr) : normal_or_noise;
        /* Process burst according to its type */
        switch (b_type) {
        case fcch_burst:
        {
          if (d_freq_offset_tag_in_fcch)
            break;
          /* Send all-zero sequence message */
          send_burst(d_burst_nr, fc_fb, GSMTAP_BURST_FCCH, input_nr);
          /* Extract frequency offset */
          const unsigned first_sample =
            ceil((GUARD_PERIOD + 2 * TAIL_BITS) * d_OSR) + 1;
          const unsigned last_sample =
            first_sample + USEFUL_BITS * d_OSR - TAIL_BITS * d_OSR;
          double freq_offset_tmp =
          compute_freq_offset(input, first_sample, last_sample);
          /* Frequency correction loop */
          d_freq_offset_setting += freq_offset_tmp;
          float phase_inc = -2 * M_PI * d_freq_offset_setting / d_sample_rate;
          d_r.set_phase_incr(exp(gr_complex(0, (d_phase_inc + phase_inc))));
          break;
        }
        case sch_burst:
        {
          int ncc, bcc;
          int t1, t2, t3;
          int rc;
          /* Get channel impulse response */
          d_c0_burst_start = get_sch_chan_imp_resp(input,
            &channel_imp_resp[0]);
          
          /* Perform MLSE detection */
          detect_burst(input, &channel_imp_resp[0],
            d_c0_burst_start, output_binary);
          /* Attempt to decode SCH burst */
          rc = decode_sch(&output_binary[3], &t1, &t2, &t3, &ncc, &bcc);
          if (rc) {
            if (++d_failed_sch >= MAX_SCH_ERRORS) {
              /* We have to resynchronize, change state */
              d_state = fcch_search;
              /* Frequency correction loop */
              pmt::pmt_t msg = pmt::make_tuple(pmt::mp("freq_offset"),
                pmt::from_double(0.0),pmt::mp("sync_loss"));
              message_port_pub(pmt::mp("measurements"), msg);
            }
            break;
          }
          /* Compose a message with GSMTAP header and bits */
          send_burst(d_burst_nr, output_binary,
            GSMTAP_BURST_SCH, input_nr, d_c0_burst_start);
          /**
           * Decoding was successful, now
           * compute offset from burst_start,
           * burst should start after a guard period.
           */
          offset = d_c0_burst_start - floor((GUARD_PERIOD) * d_OSR);
          to_consume += offset;
          d_failed_sch = 0;
          break;
        }
        case normal_burst:
        {
          float normal_corr_max;
          /**
           * Get channel impulse response for given
           * training sequence number - d_bcc
           */
          d_c0_burst_start = get_norm_chan_imp_resp(input,
            &channel_imp_resp[0], &normal_corr_max, d_bcc);
          /* Perform MLSE detection */
          detect_burst(input, &channel_imp_resp[0],
            d_c0_burst_start, output_binary);
          /* Compose a message with GSMTAP header and bits */
          send_burst(d_burst_nr, output_binary,
            GSMTAP_BURST_NORMAL, input_nr, d_c0_burst_start);
          break;
        }
        case dummy_or_normal:
        {
          unsigned int normal_burst_start, dummy_burst_start;
          float dummy_corr_max, normal_corr_max;
          dummy_burst_start = get_norm_chan_imp_resp(input,
            &channel_imp_resp[0], &dummy_corr_max, TS_DUMMY);
          normal_burst_start = get_norm_chan_imp_resp(input,
            &channel_imp_resp[0], &normal_corr_max, d_bcc);
          if (normal_corr_max > dummy_corr_max) {
            d_c0_burst_start = normal_burst_start;
            /* Perform MLSE detection */
            detect_burst(input, &channel_imp_resp[0],
              normal_burst_start, output_binary);
            /* Compose a message with GSMTAP header and bits */
            send_burst(d_burst_nr, output_binary,
              GSMTAP_BURST_NORMAL, input_nr, normal_burst_start); 
          } else {
            d_c0_burst_start = dummy_burst_start;
            /* Compose a message with GSMTAP header and bits */
            send_burst(d_burst_nr, dummy_burst,
              GSMTAP_BURST_DUMMY, input_nr, dummy_burst_start);
          }
          break;
        }
        case normal_or_noise:
        {
          std::vector<gr_complex> v(input, input + noutput_items);
          float normal_corr_max = -1e6;
//          float normal_corr_max_tmp;
          unsigned int burst_start;
          int max_tn, tseq_num;
          if (d_tseq_nums.size() == 0) {
            /**
             * There is no information about training sequence,
             * however the receiver can detect it with use of a
             * very simple algorithm based on finding
             */
            get_norm_chan_imp_resp(input, &channel_imp_resp[0],
              &normal_corr_max, 0);
            float ts_max = normal_corr_max;
            int ts_max_num = 0;
            for (int ss = 1; ss <= 7; ss++) {
              get_norm_chan_imp_resp(input, &channel_imp_resp[0],
                &normal_corr_max, ss);
              if (ts_max < normal_corr_max) {
                ts_max = normal_corr_max;
                ts_max_num = ss;
              }
            }
            d_tseq_nums.push_back(ts_max_num);
          }
          /* Choose proper training sequence number */
          tseq_num = input_nr <= d_tseq_nums.size() ?
            d_tseq_nums[input_nr - 1] : d_tseq_nums.back();
          /* Get channel impulse response */
          burst_start = get_norm_chan_imp_resp(input, &channel_imp_resp[0],
            &normal_corr_max, tseq_num);
          /* Perform MLSE detection */
          detect_burst(input, &channel_imp_resp[0],
            burst_start, output_binary);
          /* Compose a message with GSMTAP header and bits */
          send_burst(d_burst_nr, output_binary, GSMTAP_BURST_NORMAL, input_nr, burst_start);
          break;
        }
        case dummy:
          send_burst(d_burst_nr, dummy_burst, GSMTAP_BURST_DUMMY, input_nr);
          break;
        case rach_burst:
        case empty:
          /* Do nothing */
          break;
        }
        if (input_nr >= 0) {
          /* Go to the next burst */
          d_burst_nr++;
          /* Consume samples of the burst up to next guard period */
          to_consume += TS_BITS * d_OSR + d_burst_nr.get_offset();
//          consume_each(to_consume);
          d_samples_consumed += to_consume;
        }
      }
    }
	bool
    receiver_modify_impl::find_fcch_burst(const gr_complex *input,
      const int nitems, double &computed_freq_offset)
    {
      /* Circular buffer used to scan through signal to find */
      boost::circular_buffer<float>
        phase_diff_buffer(FCCH_HITS_NEEDED * d_OSR);
      boost::circular_buffer<float>::iterator buffer_iter;
      float lowest_max_min_diff;
      float phase_diff; /* Best match for FCCH burst */
      float min_phase_diff;
      float max_phase_diff;
      double best_sum = 0;
      gr_complex conjprod;
      int start_pos;
      int hit_count;
      int miss_count;
      int sample_number = 0;
      int to_consume = 0;
      bool result = false;
      bool end = false;
      /* Possible states of FCCH search algorithm */
      enum states
      {
        init,               /* initialize variables */
        search,             /* search for positive samples */
        found_something,    /* search for FCCH and the best position of it */
        fcch_found,         /* when FCCH was found */
        search_fail         /* when there is no FCCH in the input vector */
      } fcch_search_state;
      /* Set initial state */
      fcch_search_state = init;
      while (!end)
      {
        switch (fcch_search_state) {
        case init:
        {
          hit_count = 0;
          miss_count = 0;
          start_pos = -1;
          lowest_max_min_diff = 99999;
          phase_diff_buffer.clear();
          /* Change current state */
          fcch_search_state = search;
          break;
        }
        case search:
        {
          sample_number++;
          if (sample_number > nitems - FCCH_HITS_NEEDED * d_OSR) {
            /**
             * If it isn't possible to find FCCH, because
             * there is too few samples left to look into,
             * don't do anything with those samples which are left
             * and consume only those which were checked
             */
            to_consume = sample_number;
            fcch_search_state = search_fail;
            break;
          }
          phase_diff = compute_phase_diff(input[sample_number], input[sample_number - 1]);
          /**
           * If a positive phase difference was found
           * switch to state in which searches for FCCH
           */
          if (phase_diff > 0) {
              to_consume = sample_number;
              fcch_search_state = found_something;
          } else {
              fcch_search_state = search;
          }
          break;
        }
        case found_something:
        {
          if (phase_diff > 0)
            hit_count++;
          else
            miss_count++;
          if ((miss_count >= FCCH_MAX_MISSES * d_OSR)
          && (hit_count <= FCCH_HITS_NEEDED * d_OSR))
          {
            /* If miss_count exceeds limit before hit_count */
            fcch_search_state = init;
            continue;
          }
          if (((miss_count >= FCCH_MAX_MISSES * d_OSR)
          && (hit_count > FCCH_HITS_NEEDED * d_OSR))
          || (hit_count > 2 * FCCH_HITS_NEEDED * d_OSR))
          {
            /**
             * If hit_count and miss_count exceeds
             * limit then FCCH was found
             */
            fcch_search_state = fcch_found;
            continue;
          }
          if ((miss_count < FCCH_MAX_MISSES * d_OSR)
          && (hit_count > FCCH_HITS_NEEDED * d_OSR))
          {
            /**
             * Find difference between minimal and maximal 
             * element in the buffer. For FCCH this value
             * should be low. This part is searching for
             * a region where this value is lowest.
             */
            min_phase_diff = *(min_element(phase_diff_buffer.begin(),
              phase_diff_buffer.end()));
            max_phase_diff = *(max_element(phase_diff_buffer.begin(),
              phase_diff_buffer.end()));
            if (lowest_max_min_diff > max_phase_diff - min_phase_diff) {
              lowest_max_min_diff = max_phase_diff - min_phase_diff;
              start_pos = sample_number - FCCH_HITS_NEEDED
                * d_OSR - FCCH_MAX_MISSES * d_OSR;
              best_sum = 0;
              for (buffer_iter = phase_diff_buffer.begin();
                buffer_iter != (phase_diff_buffer.end());
                buffer_iter++) {
                  /* Store best value of phase offset sum */
                  best_sum += *buffer_iter - (M_PI / 2) / d_OSR;
              }
            }
          }
          /* If there is no single sample left to check */
          if (++sample_number >= nitems) {
            fcch_search_state = search_fail;
            continue;
          }
          phase_diff = compute_phase_diff(input[sample_number],input[sample_number-1]);
          if (phase_diff > 0)
              phase_diff_buffer.push_back(phase_diff);
          fcch_search_state = found_something;
          break;
        }
        case fcch_found:
        {
          /* Consume one FCCH burst */
          to_consume = start_pos + FCCH_HITS_NEEDED * d_OSR + 1;
          d_fcch_start_pos = d_counter + start_pos;
          /**
           * Compute frequency offset
           *
           * 1625000.0 / 6 - GMSK symbol rate in GSM
           */
          double phase_offset = best_sum/hit_count ;// best_sum / FCCH_HITS_NEEDED;
          double freq_offset = phase_offset * 1625000.0 / 6 / (2 * M_PI);
          computed_freq_offset = freq_offset;
          end = true;
          result = true;
          break;
        }
        case search_fail:
          end = true;
          result = false;
          break;
        }
      }
      d_counter += to_consume;
//      consume_each(to_consume);
      d_samples_consumed += to_consume;
      return result;
    }
    double
    receiver_modify_impl::compute_freq_offset(const gr_complex * input,
      unsigned first_sample, unsigned last_sample)
    {
      double phase_sum = 0;
      unsigned ii;
      double phase_diff_cnt;
      phase_diff_cnt = 0;
      for (ii = first_sample + 1; ii < last_sample; ii++)
      {
        float phase_diff = compute_phase_diff(input[ii], input[ii-1]);
        if(phase_diff > 0)
        {
            phase_diff_cnt ++;
            phase_sum += phase_diff - (M_PI / 2) / d_OSR;;
        }
      }
      if (phase_diff_cnt < (FCCH_HITS_NEEDED*d_OSR))
          phase_sum = 0;
      double phase_offset = phase_sum / phase_diff_cnt;//(last_sample - first_sample);
      double freq_offset = phase_offset * 1625000.0 / (12.0 * M_PI);
      return freq_offset;
    }
    inline float
    receiver_modify_impl::compute_phase_diff(gr_complex val1, gr_complex val2)
    {
      gr_complex conjprod = val1 * conj(val2);
      return fast_atan2f(imag(conjprod), real(conjprod));
    }
    bool
    receiver_modify_impl::reach_sch_burst(const int nitems)
    {
      /* It just consumes samples to get near to a SCH burst */
      int to_consume = 0;
      bool result = false;
      unsigned sample_nr = d_fcch_start_pos
        + (FRAME_BITS - SAFETY_MARGIN) * d_OSR;
      /* Consume samples until d_counter will be equal to sample_nr */
      if (d_counter < sample_nr) {
        to_consume = d_counter + nitems >= sample_nr ?
          sample_nr - d_counter : nitems;
      } else {
        to_consume = 0;
        result = true;
      }
      d_counter += to_consume;
//      consume_each(to_consume);
      d_samples_consumed += to_consume;
      return result;
    }
    int
    receiver_modify_impl::get_sch_chan_imp_resp(const gr_complex *input,
      gr_complex * chan_imp_resp)
    {
      std::vector<gr_complex> correlation_buffer;
      std::vector<float> window_energy_buffer;
      std::vector<float> power_buffer;
      int chan_imp_resp_center = 0;
      int strongest_window_nr;
      int burst_start;
      float energy = 0;
      int len = (SYNC_POS + SYNC_SEARCH_RANGE) * d_OSR;
      for (int ii = SYNC_POS * d_OSR; ii < len; ii++) {
        gr_complex correlation = correlate_sequence(&d_sch_training_seq[5],
          N_SYNC_BITS - 10, &input[ii]);
        correlation_buffer.push_back(correlation);
        power_buffer.push_back(std::pow(abs(correlation), 2));
      }
      /* Compute window energies */
      std::vector<float>::iterator iter = power_buffer.begin();
      while (iter != power_buffer.end()) {
        std::vector<float>::iterator iter_ii = iter;
        bool loop_end = false;
        energy = 0;
        for (int ii = 0; ii < (d_chan_imp_length) * d_OSR; ii++, iter_ii++) {
          if (iter_ii == power_buffer.end()) {
            loop_end = true;
            break;
          }
          energy += (*iter_ii);
        }
        if (loop_end)
          break;
        window_energy_buffer.push_back(energy);
        iter++;
      }
      strongest_window_nr = max_element(window_energy_buffer.begin(),
        window_energy_buffer.end()) - window_energy_buffer.begin();
#if 0
      d_channel_imp_resp.clear();
#endif
      float max_correlation = 0;
      for (int ii = 0; ii < (d_chan_imp_length) * d_OSR; ii++) {
        gr_complex correlation = correlation_buffer[strongest_window_nr + ii];
        if (abs(correlation) > max_correlation) {
          chan_imp_resp_center = ii;
          max_correlation = abs(correlation);
        }
#if 0
        d_channel_imp_resp.push_back(correlation);
#endif
        chan_imp_resp[ii] = correlation;
      }
      burst_start = strongest_window_nr + chan_imp_resp_center
        - 48 * d_OSR - 2 * d_OSR + 2 + SYNC_POS * d_OSR;
      return burst_start;
    }
    void
    receiver_modify_impl::detect_burst(const gr_complex * input,
      gr_complex * chan_imp_resp, int burst_start,
      unsigned char * output_binary)
    {
      std::vector<gr_complex> rhh_temp(CHAN_IMP_RESP_LENGTH * d_OSR);
      unsigned int stop_states[2] = {4, 12};
      gr_complex filtered_burst[BURST_SIZE];
      gr_complex rhh[CHAN_IMP_RESP_LENGTH];
      float output[BURST_SIZE];
      int start_state = 3;
      autocorrelation(chan_imp_resp, &rhh_temp[0], d_chan_imp_length*d_OSR);
      for (int ii = 0; ii < d_chan_imp_length; ii++)
        rhh[ii] = conj(rhh_temp[ii*d_OSR]);
      mafi(&input[burst_start], BURST_SIZE, chan_imp_resp,
        d_chan_imp_length * d_OSR, filtered_burst);
      viterbi_detector(filtered_burst, BURST_SIZE, rhh,
        start_state, stop_states, 2, output);
      for (int i = 0; i < BURST_SIZE; i++)
        output_binary[i] = output[i] > 0;
    }
    void
    receiver_modify_impl::gmsk_mapper(const unsigned char * input,
      int nitems, gr_complex * gmsk_output, gr_complex start_point)
    {
      gr_complex j = gr_complex(0.0, 1.0);
      gmsk_output[0] = start_point;
      int previous_symbol = 2 * input[0] - 1;
      int current_symbol;
      int encoded_symbol;
      for (int i = 1; i < nitems; i++) {
        /* Change bits representation to NRZ */
        current_symbol = 2 * input[i] - 1;
        /* Differentially encode */
        encoded_symbol = current_symbol * previous_symbol;
        /* And do GMSK mapping */
        gmsk_output[i] = j * gr_complex(encoded_symbol, 0.0)
          * gmsk_output[i-1];
        previous_symbol = current_symbol;
      }
    }
    gr_complex
    receiver_modify_impl::correlate_sequence(const gr_complex * sequence,
      int length, const gr_complex * input)
    {
      gr_complex result(0.0, 0.0);
      for (int ii = 0; ii < length; ii++)
        result += sequence[ii] * conj(input[ii * d_OSR]);
      return result / gr_complex(length, 0);
    }
    /* Computes autocorrelation for positive arguments */
    inline void
    receiver_modify_impl::autocorrelation(const gr_complex * input,
      gr_complex * out, int nitems)
    {
      for (int k = nitems - 1; k >= 0; k--) {
        out[k] = gr_complex(0, 0);
        for (int i = k; i < nitems; i++)
          out[k] += input[i] * conj(input[i - k]);
      }
    }
    inline void
    receiver_modify_impl::mafi(const gr_complex * input, int nitems,
      gr_complex * filter, int filter_length, gr_complex * output)
    {
      for (int n = 0; n < nitems; n++) {
        int a = n * d_OSR;
        output[n] = 0;
        for (int ii = 0; ii < filter_length; ii++) {
          if ((a + ii) >= nitems * d_OSR)
            break;
          output[n] += input[a + ii] * filter[ii];
        }
      }
    }
    /* Especially computations of strongest_window_nr */
    int
    receiver_modify_impl::get_norm_chan_imp_resp(const gr_complex *input,
      gr_complex *chan_imp_resp, float *corr_max, int bcc)
    {
      std::vector<gr_complex> correlation_buffer;
      std::vector<float> window_energy_buffer;
      std::vector<float> power_buffer;
     
      int search_center = (int) (TRAIN_POS + GUARD_PERIOD) * d_OSR;
      int search_start_pos = search_center + 1 - 5 * d_OSR;
      int search_stop_pos = search_center
        + d_chan_imp_length * d_OSR + 5 * d_OSR;
      for (int ii = search_start_pos; ii < search_stop_pos; ii++) {
          gr_complex correlation = correlate_sequence(
            &d_norm_training_seq[bcc][TRAIN_BEGINNING],
            N_TRAIN_BITS - 10, &input[ii]);
          correlation_buffer.push_back(correlation);
          power_buffer.push_back(std::pow(abs(correlation), 2));
      }
#if 0
      plot(power_buffer);
#endif
      /* Compute window energies */
      std::vector<float>::iterator iter = power_buffer.begin();
      while (iter != power_buffer.end()) {
        std::vector<float>::iterator iter_ii = iter;
        bool loop_end = false;
        float energy = 0;
        int len = d_chan_imp_length * d_OSR;
        for (int ii = 0; ii < len; ii++, iter_ii++) {
          if (iter_ii == power_buffer.end()) {
            loop_end = true;
            break;
          }
          energy += (*iter_ii);
        }
        if (loop_end)
          break;
        window_energy_buffer.push_back(energy);
        iter++;
      }
      /* Calculate the strongest window number */
      int strongest_window_nr = max_element(window_energy_buffer.begin(),
        window_energy_buffer.end() - d_chan_imp_length * d_OSR)
          - window_energy_buffer.begin();
      if (strongest_window_nr < 0)
        strongest_window_nr = 0;
        
      float max_correlation = 0;
      for (int ii = 0; ii < d_chan_imp_length * d_OSR; ii++) {
        gr_complex correlation = correlation_buffer[strongest_window_nr + ii];
        if (abs(correlation) > max_correlation)
          max_correlation = abs(correlation);
#if 0
        d_channel_imp_resp.push_back(correlation);
#endif
        chan_imp_resp[ii] = correlation;
      }
        
      *corr_max = max_correlation;
      /**
       * Compute first sample position, which corresponds
       * to the first sample of the impulse response
       */
      return search_start_pos + strongest_window_nr - TRAIN_POS * d_OSR;
    }
    void
    receiver_modify_impl::send_burst(burst_counter burst_nr,
      const unsigned char * burst_binary, uint8_t burst_type,
      size_t input_nr, unsigned int burst_start)
    {
      /* Buffer for GSMTAP header and burst */
      uint8_t buf[sizeof(gsmtap_hdr) + BURST_SIZE];
      uint32_t frame_number;
      uint16_t arfcn;
      uint8_t tn;
      /* Set pointers to GSMTAP header and burst inside buffer */
      struct gsmtap_hdr *tap_header = (struct gsmtap_hdr *) buf;
      uint8_t *burst = buf + sizeof(gsmtap_hdr);
     
      tap_header->version = GSMTAP_VERSION;
      tap_header->hdr_len = sizeof(gsmtap_hdr) / 4;
      tap_header->type = GSMTAP_TYPE_UM_BURST;
      tap_header->sub_type = burst_type;
      bool dl_burst = !(input_nr >= d_cell_allocation.size());
      if (dl_burst) {
        tn = static_cast<uint8_t>(d_burst_nr.get_timeslot_nr());
        frame_number = htobe32(d_burst_nr.get_frame_nr());
        arfcn = htobe16(d_cell_allocation[input_nr]);
      } else {
        input_nr -= d_cell_allocation.size();
        tn = static_cast<uint8_t>
          (d_burst_nr.subtract_timeslots(3).get_timeslot_nr());
        frame_number = htobe32(
          d_burst_nr.subtract_timeslots(3).get_frame_nr());
        arfcn = htobe16(
          d_cell_allocation[input_nr] | GSMTAP_ARFCN_F_UPLINK);
      }
      tap_header->frame_number = frame_number;
      tap_header->timeslot = tn;
      tap_header->arfcn = arfcn;
      tap_header->signal_dbm = static_cast<int8_t>(d_signal_dbm);
      tap_header->snr_db = 0; /* FIXME: Can we calculate this? */
      pmt::pmt_t pdu_header = pmt::make_dict();
      /* Add timestamp of the first sample - if available */
      if(d_rx_time_received) {
        time_spec_t time_spec_of_first_sample = d_time_samp_ref.offset_to_time(nitems_read(0)+burst_start);
        uint64_t full = time_spec_of_first_sample.get_full_secs();
        double   frac = time_spec_of_first_sample.get_frac_secs();
        pdu_header = 
          pmt::dict_add(pdu_header, pmt::mp("fn_time"), 
            pmt::cons(
              pmt::cons(pmt::from_uint64(be32toh(frame_number)), pmt::from_uint64(tn)),
              pmt::cons(pmt::from_uint64(full), pmt::from_double(frac))));
      }
      /* Copy burst to the buffer */
      memcpy(burst, burst_binary, BURST_SIZE);
      /* Allocate a new message */
      pmt::pmt_t blob = pmt::make_blob(buf, sizeof(gsmtap_hdr) + BURST_SIZE);
      pmt::pmt_t msg = pmt::cons(pdu_header, blob);
      /* Send message */
      if (input_nr == 0)
        message_port_pub(pmt::mp("C0"), msg);
      else
        message_port_pub(pmt::mp("CX"), msg);
    }
    void 
    receiver_modify_impl::configure_receiver(void)
    {
      d_channel_conf.set_multiframe_type(TIMESLOT0, multiframe_51);
      d_channel_conf.set_burst_types(TIMESLOT0, TEST51,
        sizeof(TEST51) / sizeof(unsigned), dummy_or_normal);
      d_channel_conf.set_burst_types(TIMESLOT0, TEST_CCH_FRAMES,
        sizeof(TEST_CCH_FRAMES) / sizeof(unsigned), dummy_or_normal);
      d_channel_conf.set_burst_types(TIMESLOT0, FCCH_FRAMES,
        sizeof(FCCH_FRAMES) / sizeof(unsigned), fcch_burst);
      d_channel_conf.set_burst_types(TIMESLOT0, SCH_FRAMES,
        sizeof(SCH_FRAMES) / sizeof(unsigned), sch_burst);
      d_channel_conf.set_multiframe_type(TIMESLOT1, multiframe_51);
      d_channel_conf.set_burst_types(TIMESLOT1, TEST51,
        sizeof(TEST51) / sizeof(unsigned), dummy_or_normal);
      d_channel_conf.set_multiframe_type(TIMESLOT2, multiframe_51);
      d_channel_conf.set_burst_types(TIMESLOT2, TEST51,
        sizeof(TEST51) / sizeof(unsigned), dummy_or_normal);
      d_channel_conf.set_multiframe_type(TIMESLOT3, multiframe_51);
      d_channel_conf.set_burst_types(TIMESLOT3, TEST51,
        sizeof(TEST51) / sizeof(unsigned), dummy_or_normal);
      d_channel_conf.set_multiframe_type(TIMESLOT4, multiframe_51);
      d_channel_conf.set_burst_types(TIMESLOT4, TEST51,
        sizeof(TEST51) / sizeof(unsigned), dummy_or_normal);
      d_channel_conf.set_multiframe_type(TIMESLOT5, multiframe_51);
      d_channel_conf.set_burst_types(TIMESLOT5, TEST51,
        sizeof(TEST51) / sizeof(unsigned), dummy_or_normal);
      d_channel_conf.set_multiframe_type(TIMESLOT6, multiframe_51);
      d_channel_conf.set_burst_types(TIMESLOT6, TEST51,
        sizeof(TEST51) / sizeof(unsigned), dummy_or_normal);
      d_channel_conf.set_multiframe_type(TIMESLOT7, multiframe_51);
      d_channel_conf.set_burst_types(TIMESLOT7, TEST51,
        sizeof(TEST51) / sizeof(unsigned), dummy_or_normal);
    }
    void
    receiver_modify_impl::set_cell_allocation(
      const std::vector<int> &cell_allocation)
    {
      d_cell_allocation = cell_allocation;
    }
    void
    receiver_modify_impl::set_tseq_nums(const std::vector<int> &tseq_nums)
    {
      d_tseq_nums = tseq_nums;
    }
    void
    receiver_modify_impl::reset(void)
    {
      d_state = fcch_search;
    }
    inline uint64_t
    receiver_modify_impl::resample(const gr_complex *in, uint64_t first_in_sample, gr_complex *out, uint64_t first_out_sample, uint64_t samples_to_produce)
    {
      int ii = first_in_sample;
      int oo = first_out_sample;
      while(oo < (first_out_sample+samples_to_produce)) //produce samples_to_produce number of samples
      {
        out[oo++] = d_resamp->interpolate(&in[ii], d_mu);
        double s = d_mu + d_mu_inc;
        double f = floor(s);
        int incr = (int)f;
        d_mu = s - f;
        ii += incr;
      }
      return ii-first_in_sample; //number of input samples processed
    }
    inline uint64_t
    receiver_modify_impl::cal_numinput_by_numoutput(uint64_t first_in_sample, uint64_t first_out_sample, uint64_t samples_to_produce, float &d_muu)
    {
      int ii = first_in_sample;
      int oo = first_out_sample;
      //d_mu = d_muu;
      while(oo < (first_out_sample+samples_to_produce)) //produce samples_to_produce number of samples
      {
        // out[oo++] =  d_resamp->interpolate(&in[ii], d_mu);
    	oo ++;
        double s = d_muu + d_mu_inc;
        double f = floor(s);
        int incr = (int)f;
        d_muu = s - f;
        ii += incr;
      }
      return ii-first_in_sample; //number of input samples processed
    }
  } /* namespace gsm */
} /* namespace gr */