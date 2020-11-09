/* -*- c++ -*- */
/* @file
 * @author Vadim Yanitskiy <axilirator@gmail.com>
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
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <boost/lexical_cast.hpp>

#include "grgsm/endian.h"
#include "grgsm/misc_utils/udp_socket.h"
#include "exit_channel_impl.h"

/**
 * 41-bit RACH synchronization sequence
 * GSM 05.02 Chapter 5.2.7 Access burst (AB)
 */
namespace gr {
  namespace gsm {

  exit_channel_impl::sptr
  exit_channel::make(int number_thread)
    {

      return gnuradio::get_initial_sptr
        (new exit_channel_impl(number_thread));
    }

    /*
     * The private constructor
     */
  exit_channel_impl::exit_channel_impl(int number_thread
    ) : gr::block("exit_channel_impl",
        gr::io_signature::make(1, 1, sizeof(gr_complex)),
        gr::io_signature::make(0, 0, 0))
    {
	    d_number_thread = number_thread;
	    d_i = 0;
        message_port_register_in(pmt::mp("msg_in"));
        set_msg_handler(pmt::mp("msg_in"),
        boost::bind(&exit_channel_impl::handle_msg, this, _1));
    }

    /*
     * Our virtual destructor.
     */
  exit_channel_impl::~exit_channel_impl()
    {
    }

  void
  exit_channel_impl::handle_msg(pmt::pmt_t msg)
    {
	  d_i ++;
    }
  int
  exit_channel_impl::general_work(int noutput_items,
	       gr_vector_int &ninput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items) {
	  if(d_i == d_number_thread)
		  return -1;
	  return noutput_items;
  }

  } /* namespace gsm */
} /* namespace gr */
