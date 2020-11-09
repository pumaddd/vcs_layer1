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

#ifndef INCLUDED_GSM_SYNC_IMPL_H
#define INCLUDED_GSM_SYNC_IMPL_H

#include <grgsm/receiver/sync.h>
#include <grgsm/gsmtap.h>
#include <grgsm/gsm_constants.h>
#include <receiver_config.h>
#include <vector>
#include "time_sample_ref.h"
typedef struct database{
	int id;
	std::string name_event;
	uint64_t sample_offset;
	int t1, t2, t3;
	float ppm;
} param;
namespace gr {
  namespace gsm {

    class sync_impl : public sync
    {
     private:
    	int d_numstreams;
     public:
        sync_impl(int num_streams);
        ~sync_impl();
        void forecast(int noutput_items,
  		       gr_vector_int &ninput_items_required);
        int general_work(int noutput_items,
  		       gr_vector_int &ninput_items,
  		       gr_vector_const_void_star &input_items,
  		       gr_vector_void_star &output_items);

    };
  } // namespace gsm
} // namespace gr

#endif /* INCLUDED_GSM_SUPER_BLOCK_IMPL_H */

