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
#include <stdio.h>

#include <boost/circular_buffer.hpp>
#include <boost/scoped_ptr.hpp>
#include <grgsm/endian.h>

#include "super_block_impl.h"
#include "viterbi_detector.h"
#include "sch.h"

#if 0
/* Files included for debuging */
#include "plotting/plotting.hpp"
#include <pthread.h>
#include <iomanip>
#endif

#define SYNC_SEARCH_RANGE 30

namespace gr
{
  namespace gsm
  {
    /* The public constructor */
  	super_block::sptr
	super_block::make(
			int osr, const std::vector<int> &cell_allocation, int maio, int hsn,
			int tseq, double sample_rate, std::string table_name, uint64_t id_start, uint64_t index)
    {
      return gnuradio::get_initial_sptr
        (new super_block_impl(osr, cell_allocation, maio, hsn, tseq, sample_rate, table_name, id_start, index));
    }

    /* The private constructor */
  	super_block_impl::super_block_impl(
  			int osr, const std::vector<int> &cell_allocation, int maio, int hsn,
			int tseq, double sample_rate, std::string table_name, uint64_t id_start, uint64_t index
    ) : gr::block("super_block",
          gr::io_signature::make(1, -1, sizeof(gr_complex)),
          gr::io_signature::make(0, 0, sizeof(gr_complex))),
        d_samples_consumed(0),
        d_rx_time_received(false),
        d_time_samp_ref(GSM_SYMBOL_RATE * osr),
        d_OSR(osr),
        d_chan_imp_length(CHAN_IMP_RESP_LENGTH),
		d_ma(cell_allocation),
        d_burst_nr(osr),
        d_signal_dbm(-120),
		d_maio(maio),
		d_hsn(hsn),
		d_tseq(tseq),
    	d_sample_rate(sample_rate),
  		d_mai(0),
  		d_table_name(table_name),
  		d_id_start(id_start),
		d_index(index)
    {
  	  SAMPLERATE_OUT = (long double)(1625000.0*d_OSR/ 6);
      set_output_multiple(floor((TS_BITS + 2 * (GUARD_PERIOD)) * d_OSR));
      d_sample_per_burst = (TS_BITS + GUARD_FRACTIONAL) * d_OSR;
      d_burst_start = 0;
      /* Prepare bits of training sequences */
      for (int i = 0; i < TRAIN_SEQ_NUM; i++) {
        gr_complex startpoint = train_seq[i][0] == 0 ?
          gr_complex(1.0, 0.0) : gr_complex(-1.0, 0.0);
        gmsk_mapper(train_seq[i], N_TRAIN_BITS,
          d_norm_training_seq[i], startpoint);
      }

      /* Register output ports */
      message_port_register_out(pmt::mp("CX"));
      /**
       * Configure the receiver,
       * i.e. tell it where to find which burst type
       */
      configure_receiver();
      d_mu_inc = d_sample_rate / SAMPLERATE_OUT;
	  d_narfcn = d_ma.size();
      d_frame_number = 0;
      d_flag_decoder = false;
      d_sample_fractional = 0;
#ifdef SQLITE3
	  int rc = sqlite3_open("/etc/data/test.db", &database);

	   if( rc ) {
	      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(database));
	   } else {
	      fprintf(stderr, "Opened database successfully\n");
	   }
	   sqlite3_exec(database, "pragma journal_mode = WAL", NULL, NULL, NULL);
#else
	   try {
	  	  mysql_driver = get_driver_instance();
	  	  mysql_con = mysql_driver->connect(d_grd_config.get_database(), d_grd_config.get_username(), grd_config_impl().get_password());
	  	  /* Connect to the MySQL test database */
	  	  mysql_con->setSchema("test.db");
	  	  mysql_stmt = mysql_con->createStatement();
	   }
	   catch (sql::SQLException &e) {
	     	  std::cout << "ERROR: SQLException in " << __FILE__;
	     	  std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
	     	  std::cout << "ERROR: " << e.what();
	     	  std::cout << " (MySQL error code: " << e.getErrorCode();
	     	  std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
	   }
#endif

       find_db_via_id(database, d_id_start);
       d_sample = d_sample_start = db.sample_offset;
    }

    /* Our virtual destructor */
  	super_block_impl::~super_block_impl() {
#ifdef SQLITE3
    	sqlite3_close(database);
#else
    	mysql_con->close();
    	mysql_stmt->close();
    	delete mysql_con;
    	delete mysql_stmt;
#endif
  	}

  	unsigned char RNTABLE_01[114] = {
	            48, 98, 63, 1, 36, 95, 78, 102, 94, 73, \
	            0, 64, 25, 81, 76, 59, 124, 23, 104, 100, \
	            101, 47, 118, 85, 18, 56, 96, 86, 54, 2, \
	            80, 34, 127, 13, 6, 89, 57, 103, 12, 74, \
	            55, 111, 75, 38, 109, 71, 112, 29, 11, 88, \
	            87, 19, 3, 68, 110, 26, 33, 31, 8, 45, \
	            82, 58, 40, 107, 32, 5, 106, 92, 62, 67, \
	            77, 108, 122, 37, 60, 66, 121, 42, 51, 126, \
	            117, 114, 4, 90, 43, 52, 53, 113, 120, 72, \
	            16, 49, 7, 79, 119, 61, 22, 84, 9, 97, \
	            91, 15, 21, 24, 46, 39, 93, 105, 65, 70, \
	            125, 99, 17, 123 \
	  };
    void
	super_block_impl::forecast(int noutput_items,
                            gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++) {
        ninput_items_required[i] = 2 * noutput_items;
      }

    }

    int
	super_block_impl::general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
      gr_complex *in;
      long double start = nitems_read(0) + (long double)d_sample/d_mu_inc;
	  d_samples_consumed = 0;
	  int frame_number = 0;
      int process_in_offset = 0;
      long double a = (long double)d_sample_start/d_mu_inc;
      if (start < a)
      {
    	  if ((a - start) >= floor(d_sample_per_burst)) {
    	      consume_each(floor(d_sample_per_burst));
    		  return floor(d_sample_per_burst);
    	  }
    	  else {
    		  consume_each((uint64_t)(d_sample_start/d_mu_inc - start));
    		  return ((uint64_t)(d_sample_start/d_mu_inc - start));
    	  }
      }
      a = db.sample_offset/d_mu_inc - start;
      if (a <= d_sample_per_burst && a >= -1) {
    	  if(db.name_event == 1){
    		  	  find_db_via_id(database, (db.id + 1));
    	  }
      }
      a = (long double)db.sample_offset/d_mu_inc - start;
      if (a < d_sample_per_burst && a >= -1) {
      	  if(db.name_event == 2) {
        	  d_flag_decoder = true;
      		  if(a < d_sample_per_burst/2) {
            	  d_burst_nr.set(db.t1, db.t2, db.t3, 0);
            	  d_sample_fractional = a -(int)a;
            	  if(d_sample_fractional >=1) {
            		  process_in_offset = (int)a + 1;
            		  d_sample_fractional = d_sample_fractional -1;
            	  } else
            		  process_in_offset = (int)a;
      		  } else {
      			  d_burst_nr.set(db.t1, db.t2, db.t3, 0);
      			  d_burst_nr--;
      			  d_sample_fractional = a -(int)a;
            	  if(d_sample_fractional >=1) {
            		  process_in_offset = -ceil(d_sample_per_burst) + (int)a + 1;
            		  d_sample_fractional = d_sample_fractional -1;
            	  } else
            		  process_in_offset = -ceil(d_sample_per_burst) + (int)a;
      		  }
   			 find_db_via_id(database, (db.id + 1));
      	  }
      }
      for(int i = 0; i < 2; i++) {
    	  in = (gr_complex *) input_items[i];
    	  if(i==0){
    		  synchronized_handler(in, noutput_items, false, 0);
    	  }
    	  if(i==1) {
    		  synchronized_handler(in, noutput_items, true, 1);
    	  }
      }

      consume_each(d_samples_consumed + process_in_offset);
      return (d_samples_consumed);
    }

    void
	super_block_impl::synchronized_handler(gr_complex *input, int noutput_items, bool uplink, int i)
    {
      /**
       * In this state receiver is synchronized and it processes
       * bursts according to burst type for given burst number
       */
      std::vector<gr_complex> channel_imp_resp(CHAN_IMP_RESP_LENGTH * d_OSR);
      //size_t inputs_to_process = d_cell_allocation.size();
      unsigned char output_binary[BURST_SIZE];
      burst_type b_type;
      int to_consume = 0;
      int offset = 0;
      uint32_t frame_number;
      size_t input_nr = 0;
      double signal_pwr = 0;
      double isvalid;
      uint8_t snr;

        for (int ii = GUARD_PERIOD; ii < TS_BITS; ii++)
          signal_pwr += abs(input[ii]) * abs(input[ii]);

        signal_pwr = signal_pwr / (TS_BITS);
        d_signal_dbm = round(10 * log10(signal_pwr / 50));

        b_type = normal_or_noise;
        // std::cout << d_burst_nr.get_timeslot_nr() << "\t signal_pwr: " <<signal_pwr << "\tsignal_dbm: " << d_signal_dbm << std::endl;
        /* Process burst according to its type */
          std::vector<gr_complex> v(input, input + noutput_items);
          float normal_corr_max, dummy_corr_max;
//          float normal_corr_max_tmp;
          int burst_start, dummy_burst_start;
          int max_tn, tseq_num;

          /* Get channel impulse response */
          dummy_burst_start = get_norm_chan_imp_resp(input,
            &channel_imp_resp[0], &dummy_corr_max, TS_DUMMY, isvalid);
          burst_start = get_norm_chan_imp_resp(input, &channel_imp_resp[0],
            &normal_corr_max, d_tseq, isvalid);
          if(normal_corr_max > dummy_corr_max) {
          /* Perform MLSE detection */
          snr = isvalid > 30 ? 255: isvalid*255/30;
//          std::cout<<snr<<std::endl;
//          std::cout<<"SNR\t"<<snr<<std::endl;
          detect_burst(input, &channel_imp_resp[0],
            burst_start, output_binary);

          /* Compose a message with GSMTAP header and bits */
//          std::cout<<"debug!!!!!!!!\t"<<snr<<std::endl;
          if(d_flag_decoder)
        	  send_burst(d_burst_nr, output_binary, GSMTAP_BURST_NORMAL, input_nr, burst_start,snr, uplink);
          } else {
        	  send_burst(d_burst_nr, dummy_burst, GSMTAP_BURST_DUMMY, input_nr, dummy_burst_start, 0, uplink);
          }
          if(i == 1) {
          /* Go to the next burst */
        	d_burst_nr++;
          /* Consume samples of the burst up to next guard period */
        	to_consume += TS_BITS * d_OSR + d_burst_nr.get_offset();
        	d_samples_consumed += to_consume;
          }
    }

    void
	super_block_impl::detect_burst(const gr_complex * input,
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
    super_block_impl::gmsk_mapper(const unsigned char * input,
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
    super_block_impl::correlate_sequence(const gr_complex * sequence,
      int length, const gr_complex * input)
    {
      gr_complex result(0.0, 0.0);

      for (int ii = 0; ii < length; ii++)
        result += sequence[ii] * conj(input[ii * d_OSR]);

      return result / gr_complex(length, 0);
    }

    /* Computes autocorrelation for positive arguments */
    inline void
    super_block_impl::autocorrelation(const gr_complex * input,
      gr_complex * out, int nitems)
    {
      for (int k = nitems - 1; k >= 0; k--) {
        out[k] = gr_complex(0, 0);
        for (int i = k; i < nitems; i++)
          out[k] += input[i] * conj(input[i - k]);
      }
    }

    inline void
    super_block_impl::mafi(const gr_complex * input, int nitems,
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
    super_block_impl::get_norm_chan_imp_resp(const gr_complex *input,
      gr_complex *chan_imp_resp, float *corr_max, int bcc, double &valid)
    {
        std::vector<gr_complex> correlation_buffer;
        std::vector<float> window_energy_buffer;
        std::vector<float> power_buffer;

        int search_center = (int) (TRAIN_POS + GUARD_PERIOD) * d_OSR;
        int search_start_pos = search_center + 1 - 5 * d_OSR;
        int search_stop_pos = search_center
  //		int search_start_pos = 0;
  //		int search_stop_pos = (int) (TRAIN_POS + GUARD_PERIOD + 5) * d_OSR;
          + d_chan_imp_length * d_OSR + 5 * d_OSR;

        for (int ii = search_start_pos; ii < search_stop_pos; ii++) {
            gr_complex correlation = correlate_sequence(
              &d_norm_training_seq[bcc][TRAIN_BEGINNING],
              N_TRAIN_BITS - 10, &input[ii]);
            correlation_buffer.push_back(correlation);
            // power_buffer.push_back(std::pow(abs(correlation), 2));
            power_buffer.push_back(abs(correlation) * abs(correlation));
        }

  #if USE_PLOT
        plot(power_buffer);
  #endif

        /* Compute window energies */
        std::vector<float>::iterator iter = power_buffer.begin();
        while (iter != power_buffer.end()) {
          std::vector<float>::iterator iter_ii = iter;
          bool loop_end = false;
          float energy = 0;

          int len = d_chan_imp_length * d_OSR;
          int ii;
          for ( ii = 0; ii < len; ii++, iter_ii++) {
            if (iter_ii == power_buffer.end()) {
              loop_end = true;
              break;
            }

            energy += (*iter_ii);
          }

          if (loop_end)
            break;
          // std::cout << "len: "<< power_buffer.size() << "\tii: "<< ii << "\tenergy: "<< energy/(d_chan_imp_length * d_OSR)<<  std::endl;
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
        int max_correl_pos = -1;
        for (int ii = 0; ii < d_chan_imp_length * d_OSR; ii++) {
          gr_complex correlation = correlation_buffer[strongest_window_nr + ii];
          if (abs(correlation) > max_correlation)
          {
            max_correlation = abs(correlation);
            max_correl_pos = ii;
          }
  #if 0
          d_channel_imp_resp.push_back(correlation);
  #endif

          chan_imp_resp[ii] = correlation;
        }


        *corr_max = max_correlation;
        float rms;
        // compute avg window power
        {
  		  int num = 0;
  		  // gr_complex *peak;
  		  int peak;
  		  float avg = 0.0;
  		  peak = strongest_window_nr ;

  		  for (int i = 2 * d_OSR; i <= 5 * d_OSR; i++) {
  			if ((peak - i) >= 0) {
  			  avg += window_energy_buffer[peak - i];
  			  num++;
  			  //std::cout << "+++++++ num: " << num << std::endl;
  			}
  			if (peak + i < (window_energy_buffer.size() - d_chan_imp_length * d_OSR)) {
  			  avg += window_energy_buffer[peak + i];
  			  num++;
  			  //std::cout << "------ num: " << num << std::endl;
  			}
  		  }

  		  if (num < 2)
  		  {
  			  rms = 1;
  			  valid = false;
  		  }
  		  else
  		  {
  			  rms = (avg / (float) num);
  			  float _valid = std::abs(window_energy_buffer[strongest_window_nr]/rms) ;//> 5;
  			  valid = _valid;//  >=5 ;
  			  //valid = _valid >=5 ;
  			  // std::cout << "strongest_window_nr: " << strongest_window_nr <<"\trms: " << rms/(d_chan_imp_length * d_OSR) << "\twindow_energy_buffer: "  << window_energy_buffer[strongest_window_nr]/(d_chan_imp_length * d_OSR) <<"\tvalid: " << _valid << "\tdbm: "<< round(10 * log10(window_energy_buffer[strongest_window_nr] / (50* d_chan_imp_length * d_OSR)))<< std::endl;
  		  }
        }

        /**
         * Compute first sample position, which corresponds
         * to the first sample of the impulse response
         */
        int burst_start = search_start_pos + strongest_window_nr - TRAIN_POS * d_OSR;
        float noise = 0;
        float signal = 0;
        float amplitude_average = 0;
        float amplitude = 0;
        for(int i = burst_start; i < burst_start + BURST_SIZE; i++) {
        	signal += abs(input[i]) * abs(input[i]);
        	amplitude += abs(input[i]);
        }
        amplitude_average = amplitude / (BURST_SIZE);
        for(int i = burst_start; i < burst_start + BURST_SIZE; i++) {
                	noise += (abs(input[i]) - amplitude_average) * (abs(input[i]) - amplitude_average);
        }
        valid = 15*log10(signal/(2*noise));

        return search_start_pos + strongest_window_nr - TRAIN_POS * d_OSR; // (int) ( GUARD_PERIOD) * d_OSR -19 ;
    }


    void
    super_block_impl::send_burst(burst_counter burst_nr,
      const unsigned char * burst_binary, uint8_t burst_type,
      size_t input_nr, unsigned int burst_start, unsigned int snr, bool uplink)
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

      if(uplink) {
          frame_number  = d_burst_nr.subtract_timeslots(3).get_frame_nr();
//    	  std::cout<<"uplink: "<<frame_number<<std::endl;
          d_mai = calculate_ma_sfh(d_maio, d_hsn, d_narfcn, frame_number);
          tap_header->frame_number = htobe32(d_burst_nr.subtract_timeslots(3).get_frame_nr());
    	  tap_header->arfcn = htobe16(d_ma[d_mai] | GSMTAP_ARFCN_F_UPLINK);
        // if (d_burst_nr.get_timeslot_nr() == 7)
          // std::cout<<"Uplink \t"<<"snr: "<<snr<<"\tma: "<<d_ma[d_mai]<<std::endl;
    	  tap_header->timeslot = static_cast<uint8_t>(d_burst_nr.subtract_timeslots(3).get_timeslot_nr());
      }
      else {
          frame_number  = d_burst_nr.get_frame_nr();
//          std::cout<<frame_number<<"snr: "<<snr<<std::endl;
          d_mai = calculate_ma_sfh(d_maio, d_hsn, d_narfcn, frame_number);
          // if (d_burst_nr.get_timeslot_nr() == 7)
              // std::cout<<"Downlink \t"<<"snr: "<<snr<<"\tma: "<<d_ma[d_mai]<<std::endl;

          tap_header->frame_number = htobe32(d_burst_nr.get_frame_nr());
    	  tap_header->arfcn = htobe16(d_ma[d_mai]);
    	  tap_header->timeslot = static_cast<uint8_t>(d_burst_nr.get_timeslot_nr());
      }
      tap_header->signal_dbm = static_cast<int8_t>(d_signal_dbm);
      tap_header->snr_db = snr; /* FIXME: Can we calculate this? */

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
      message_port_pub(pmt::mp("CX"), msg);
    }

    void
    super_block_impl::configure_receiver(void)
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
#ifdef SQLITE3
 	int
	super_block_impl::callback(void *_db, int argc, char **argv, char **azColName) {
 		 int i;
 		 param* db = (param*) _db;
 		 for(i = 0; i<argc; i++) {
 			 if((strcmp(azColName[i], "NameEvent")) == 0) {
 				 db->name_event = argv[i];
     	  //printf("DEBUG: Name event: %s \n", db.name_event.c_str());
 			 }
 			 if((strcmp(azColName[i],"ID")) == 0) {
 				 db->id = std::strtoull(argv[i],NULL,0);
 			 }
 			 if((strcmp(azColName[i],"SampleOffset")) == 0) {
     	   db->sample_offset = std::strtoull(argv[i],NULL,0);
     	   //std::cout<<"sample_offset: "<<db.sample_offset<<std::endl;
       }
       if((strcmp(azColName[i], "T1")) == 0) {
     	   db->t1 = atoi(argv[i]);
       }
       if((strcmp(azColName[i], "T2")) == 0) {
     	   db->t2 = atoi(argv[i]);
       }
       if((strcmp(azColName[i], "T3")) == 0) {
     	   db->t3 = atoi(argv[i]);
       }
       if((strcmp(azColName[i], "PPM")) == 0) {
     	   db->ppm = atof(argv[i]);
       }

    }
    return 0;
  }
#endif
    int
	super_block_impl::find_db_via_id(sqlite3* database, int ID){
#ifdef SQLITE3
      int rc;
      char *zErrMsg = 0;
      char sql[1000] = {0};
      sprintf(sql, "SELECT * FROM \'%s\' WHERE ID = %d", d_table_name.c_str(), ID);
      while(1){
      rc = sqlite3_exec(database, sql, callback, (void*)&this->db, &zErrMsg);
      if( rc != SQLITE_OK ) {
    	usleep(1);
    	perror("super_block_impl:Read database fail!!!");
        sqlite3_free(zErrMsg);
      } else {
    	  break;
      }
      }
#else
      try {
    //         sql::Driver *mysql_driver;
    //         sql::Connection *mysql_con;
    //         sql::Statement *mysql_stmt;
     	 sql::ResultSet *res = NULL;
     	 char *zErrMsg = 0;
     	 char sql[1000] = {0};
     	 sprintf(sql, "SELECT * FROM `%s` WHERE ID = %llu and Index_round = %llu", d_table_name.c_str(), ID, d_index);
     	 res = mysql_stmt->executeQuery(sql);
     	 while (res->next()) {
     		db.id = std::strtoull(res->getString(1).c_str(),NULL,0);
     		db.name_event = atoi(res->getString(2).c_str());
     		db.sample_offset = std::strtoull(res->getString(3).c_str(),NULL,0);
     		db.t1 = atoi(res->getString(4).c_str());
     		db.t2 = atoi(res->getString(5).c_str());
     		db.t3 = atoi(res->getString(6).c_str());
     		db.ppm = atof(res->getString(7).c_str());
     	  }
     	 delete res;
       }
       catch (sql::SQLException &e) {
     	  std::cout << "ERROR: SQLException in " << __FILE__;
     	  std::cout << " (" << __func__<< ") on line " << __LINE__ << std::endl;
     	  std::cout << "ERROR: " << e.what();
     	  std::cout << " (MySQL error code: " << e.getErrorCode();
     	  std::cout << ", SQLState: " << e.getSQLState() << ")" << std::endl;
       }

#endif
      return 0;
    }
    int
	super_block_impl::calculate_ma_sfh(int maio, int hsn, int n, int fn)
    {
        int mai = 0;
        int s = 0;
        int nbin = floor(log2(n) + 1);
        int t1 = fn / 1326;
        int t2 = fn % 26;
        int t3 = fn % 51;

        if (hsn == 0)
            mai = (fn + maio) % n;
        else {
            int t1r = t1 % 64;
            int m = t2 + RNTABLE_01[(hsn ^ t1r) + t3];
            int mprim = m % (1 << nbin);
            int tprim = t3 % (1 << nbin);

            if (mprim < n)
                s = mprim;
            else
                s = (mprim + tprim) % n;

            mai = (s + maio) % n;
        }

        return (mai);
    }
    void
	super_block_impl::set_ma(std::vector<int> ma) {
    	d_ma.clear();
    	d_ma = ma;
    }
    void
	super_block_impl::set_maio(int maio) {
    	d_maio = maio;
    }
    void
	super_block_impl::set_hsn(int hsn) {
    	d_hsn = hsn;
    }
    void
	super_block_impl::set_id_start(int id_start) {
    	d_id_start = id_start;
        find_db_via_id(database, d_id_start);
        d_sample_start = db.sample_offset;
        d_sample = db.sample_offset;
    }
    void
	super_block_impl::set_sample_start(int sample_start) {
    	d_sample = sample_start;
    }
    void
	super_block_impl::set_sample_rate(float set_sample_rate) {
    	d_sample_rate = set_sample_rate;
    }
    void
	super_block_impl::set_tseq(int tseq) {
    	d_tseq = tseq;
    }

  } /* namespace gsm */
} /* namespace gr */
