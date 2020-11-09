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

#ifndef INCLUDED_FILTER_PFB_CHANNELIZER_SCANNER_H
#define	INCLUDED_FILTER_PFB_CHANNELIZER_SCANNER_H

#include <gnuradio/filter/pfb_channelizer_ccf.h>
#include <gnuradio/filter/polyphase_filterbank.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/thread/thread.h>
//#include <gnuradio/sync_block.h>
#include <gnuradio/block.h>
#include <grgsm/api.h>

namespace gr {
  namespace gsm {

    //class GRGSM_API pfb_channelizer_scanner : virtual public sync_block
    class GRGSM_API pfb_channelizer_scanner : virtual public block
    {
    public:
    typedef boost::shared_ptr<pfb_channelizer_scanner> sptr;
	static sptr make(unsigned int nfilts,
                              const std::vector<float> &taps,
                              float oversample_rate,
                              uint64_t map 
                              ); 
      virtual void set_taps(const std::vector<float> &taps) = 0;
      virtual void print_taps() = 0;
      virtual std::vector<std::vector<float> > taps() const = 0;

      virtual void set_channel_map(const std::vector<int> &map) = 0;
      virtual std::vector<int> channel_map() const = 0;

    }; /* namespace gsm */
  } /* namespace gr */
}
#endif
