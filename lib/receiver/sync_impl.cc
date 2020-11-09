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

#include "sync_impl.h"
#include "viterbi_detector.h"


namespace gr
{
  namespace gsm
  {
    /* The public constructor */
  sync::sptr
  sync::make(int num_streams)
    {
      return gnuradio::get_initial_sptr
        (new sync_impl(num_streams));
    }

    /* The private constructor */
  sync_impl::sync_impl(int num_streams
    ) : gr::block("sync",
          gr::io_signature::make(1, -1, sizeof(gr_complex)),
          gr::io_signature::make(1, -1, sizeof(gr_complex)))
    {
	  d_numstreams = 2 * num_streams;
    }

    /* Our virtual destructor */
  sync_impl::~sync_impl() {
  	}

    void
	sync_impl::forecast(int noutput_items,
                            gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for(unsigned i=0; i < ninputs; i++) {
        ninput_items_required[i] = noutput_items;
      }

    }

    int
	sync_impl::general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
    {
      for(int i = 0; i < d_numstreams; i++)
      {
    	  gr_complex* in = (gr_complex*)input_items[i];
    	  gr_complex* out = (gr_complex*)output_items[i];
    	  memcpy(out, in, noutput_items * 8);
      }
      consume_each(noutput_items);
      return (noutput_items);
    }



  } /* namespace gsm */
} /* namespace gr */
