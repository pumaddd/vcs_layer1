/* -*- c++ -*- */
/*
 * Copyright 2009,2010,2013 Free Software Foundation, Inc.
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


#ifndef INCLUDED_CONTROL_ARB_RESAMPLER_CCF_IMPL_H
#define	INCLUDED_CONTROL_ARB_RESAMPLER_CCF_IMPL_H

#include <grgsm/receiver/control_arb_resampler_ccf.h>
#include <gnuradio/filter/pfb_arb_resampler.h>
#include <gnuradio/thread/thread.h>
#include <gnuradio/filter/firdes.h>
#include <grgsm/gsm_constants.h>
#include "grd_config_impl.h"
namespace gr {
  namespace gsm {

    class FILTER_API control_arb_resampler_ccf_impl : public control_arb_resampler_ccf
    {
    private:
      gr::filter::kernel::pfb_arb_resampler_ccf *d_resamp;
      bool d_updated;
      gr::thread::mutex d_mutex; // mutex to protect set/work access
      double d_rate;
      int d_last_required;
      grd_config_impl d_grd_config;
    public:
      control_arb_resampler_ccf_impl(double rate,
				 const std::vector<float> &taps,
				 unsigned int filter_size);

      ~control_arb_resampler_ccf_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);

      void set_taps(const std::vector<float> &taps);
      std::vector<std::vector<float> > taps() const;
      void print_taps();
      std::vector<float> d_taps;
      void set_rate(double rate);
      void set_phase(float ph);
      float phase() const;
      int d_filter_size;
      unsigned int interpolation_rate() const;
      unsigned int decimation_rate() const;
      float fractional_rate() const;
      unsigned int taps_per_filter() const;

      int group_delay() const;
      float phase_offset(float freq, float fs);

      int general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items);
    };

  } /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_PFB_ARB_RESAMPLER_CCF_IMPL_H */
