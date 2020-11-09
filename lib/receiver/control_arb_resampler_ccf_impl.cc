/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2012 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "control_arb_resampler_ccf_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>

namespace gr {
  namespace gsm {

    control_arb_resampler_ccf::sptr
    control_arb_resampler_ccf::make(double rate,
				const std::vector<float> &taps,
				unsigned int filter_size)
    {
      return gnuradio::get_initial_sptr
	(new control_arb_resampler_ccf_impl(rate, taps, filter_size));
    }


    control_arb_resampler_ccf_impl::control_arb_resampler_ccf_impl(double rate,
							   const std::vector<float> &taps,
							   unsigned int filter_size)
      : block("control_arb_resampler_ccf",
              io_signature::make(1, 1, sizeof(gr_complex)),
              io_signature::make(1, 1, sizeof(gr_complex))),
			  d_taps(taps)
    {
      d_rate = rate;
      d_updated = false;
      d_filter_size = filter_size;
      d_last_required = 0;
      d_resamp = new gr::filter::kernel::pfb_arb_resampler_ccf(rate, taps, filter_size);
      set_history(d_resamp->taps_per_filter());
      set_relative_rate(rate);
      set_output_multiple(floor((TS_BITS + 2 * GUARD_PERIOD) * d_grd_config.get_osr()));
      enable_update_rate(true);
      // d_resamp->print_taps();
    }

    control_arb_resampler_ccf_impl::~control_arb_resampler_ccf_impl()
    {
      delete d_resamp;
    }

    void
    control_arb_resampler_ccf_impl::forecast(int noutput_items,
                                         gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      if(noutput_items / relative_rate() < 1) {
        for(unsigned i = 0; i < ninputs; i++)
        {
          ninput_items_required[i] = relative_rate() + history() - 1;
          d_last_required = ninput_items_required[i];
        }
      }
      else {
        for(unsigned i = 0; i < ninputs; i++)
        {
          ninput_items_required[i] = noutput_items/relative_rate() + history() - 1;
          d_last_required = ninput_items_required[i];
        }
      }

    }

    void
    control_arb_resampler_ccf_impl::set_taps(const std::vector<float> &taps)
    {
      gr::thread::scoped_lock guard(d_mutex);

      d_resamp->set_taps(taps);
      set_history(d_resamp->taps_per_filter());
      d_updated = true;
    }

    std::vector<std::vector<float> >
    control_arb_resampler_ccf_impl::taps() const
    {
      return d_resamp->taps();
    }

    void
    control_arb_resampler_ccf_impl::print_taps()
    {
      d_resamp->print_taps();
    }

    void
    control_arb_resampler_ccf_impl::set_rate(double rate)
    {
      gr::thread::scoped_lock guard(d_mutex);

      d_resamp->set_rate(rate);
      set_relative_rate(rate);
    }

    void
    control_arb_resampler_ccf_impl::set_phase(float ph)
    {
      gr::thread::scoped_lock guard(d_mutex);
      d_resamp->set_phase(ph);
    }

    float
    control_arb_resampler_ccf_impl::phase() const
    {
      return d_resamp->phase();
    }

    unsigned int
    control_arb_resampler_ccf_impl::interpolation_rate() const
    {
      return d_resamp->interpolation_rate();
    }

    unsigned int
    control_arb_resampler_ccf_impl::decimation_rate() const
    {
      return d_resamp->decimation_rate();
    }

    float
    control_arb_resampler_ccf_impl::fractional_rate() const
    {
      return d_resamp->fractional_rate();
    }

    unsigned int
    control_arb_resampler_ccf_impl::taps_per_filter() const
    {
      return d_resamp->taps_per_filter();
    }

    int
    control_arb_resampler_ccf_impl::group_delay() const
    {
      return d_resamp->group_delay();
    }

    float
    control_arb_resampler_ccf_impl::phase_offset(float freq, float fs)
    {
      return d_resamp->phase_offset(freq, fs);
    }

    int
    control_arb_resampler_ccf_impl::general_work(int noutput_items,
					     gr_vector_int &ninput_items,
					     gr_vector_const_void_star &input_items,
					     gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_mutex);

      gr_complex *in = (gr_complex*)input_items[0];
      gr_complex *out = (gr_complex*)output_items[0];

//      std::cout<<nitems_read(0)<<"\control_arb_resampler_ccf_impl:: \t"<<std::endl;
      uint64_t start = nitems_read(0);
      if(d_updated) {
		d_updated = false;
		return 0;		     // history requirements may have changed.
      }
		  std::vector<tag_t> set_tags;
		  pmt::pmt_t  key = pmt::string_to_symbol("set_arfcn");
		  get_tags_in_window(set_tags, 0, 0, d_last_required*2, key);
		  for(std::vector<tag_t>::iterator i_tag = set_tags.begin(); i_tag < set_tags.end(); i_tag++){
			int arfcn = pmt::to_uint64(i_tag->value);
			pmt::pmt_t messages = pmt::make_dict();
			messages = dict_add(messages, pmt::string_to_symbol("set_arfcn"),
			pmt::from_uint64(arfcn));
			pmt::pmt_t klist(pmt::dict_keys(messages));
			for (size_t i = 0; i < pmt::length(klist); i++) {
			  pmt::pmt_t k(pmt::nth(i, klist));
			  pmt::pmt_t v(pmt::dict_ref(messages, k, pmt::PMT_NIL));
			  add_item_tag(0, nitems_written(0), k, v, alias_pmt());
			}
			remove_item_tag(0, i_tag->offset, i_tag->key, i_tag->value, i_tag->srcid);

			break;
		  }
		  key = pmt::string_to_symbol("set_arfcn_priority");
		  get_tags_in_window(set_tags, 0, 0, d_last_required*2, key);
		  for(std::vector<tag_t>::iterator i_tag = set_tags.begin(); i_tag < set_tags.end(); i_tag++){
			int arfcn = pmt::to_uint64(i_tag->value);
			pmt::pmt_t messages = pmt::make_dict();
			messages = dict_add(messages, pmt::string_to_symbol("set_arfcn_priority"),
			pmt::from_uint64(arfcn));
			pmt::pmt_t klist(pmt::dict_keys(messages));
			for (size_t i = 0; i < pmt::length(klist); i++) {
			  pmt::pmt_t k(pmt::nth(i, klist));
			  pmt::pmt_t v(pmt::dict_ref(messages, k, pmt::PMT_NIL));
			  add_item_tag(0, nitems_written(0), k, v, alias_pmt());
			}
			 remove_item_tag(0, i_tag->offset, i_tag->key, i_tag->value, i_tag->srcid);
			break;
		  }

		  key = pmt::string_to_symbol("set_resampling_rate");
		  get_tags_in_window(set_tags, 0, 0, d_last_required*2, key);
		  for(std::vector<tag_t>::iterator i_tag = set_tags.begin(); i_tag < set_tags.end(); i_tag++){
			uint64_t tag_offset_rel = i_tag->offset - nitems_read(0);

			int nitems_reads;
			int processed = d_resamp->filter(out, in, tag_offset_rel, nitems_reads);


			d_rate = pmt::to_double(i_tag->value);
			delete d_resamp;
			d_taps.clear();
			d_taps = gr::filter::firdes::low_pass_2(d_filter_size,d_filter_size, d_rate*0.8/2,  d_rate*0.8/4,100, gr::filter::firdes::WIN_BLACKMAN_HARRIS);
			d_resamp = new gr::filter::kernel::pfb_arb_resampler_ccf(d_rate, d_taps, d_filter_size);
			set_relative_rate(d_rate);
			enable_update_rate(true);
			remove_item_tag(0, i_tag->offset, i_tag->key, i_tag->value, i_tag->srcid);
			consume_each(nitems_reads);
			return processed;
		  }

      int nitems_read;
      int nitems = floorf((float)noutput_items / relative_rate());
      int processed = d_resamp->filter(out, in, nitems, nitems_read);
//      if(nitems != nitems_read)
//    	  std::cout<<nitems_read<<"\t"<<noutput_items<<"\t"<<nitems<<std::endl;
//      std::cout<<start<<"\t"<<nitems_read<<"\t"<<noutput_items/d_rate<<"\tcontrol_arb_resampler_ccf_impl:: \t"<<std::endl;
      consume_each(nitems_read);
      return processed;
    }

  } /* namespace filter */
} /* namespace gr */
