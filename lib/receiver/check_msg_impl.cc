/* -*- c++ -*- */
/*
 * @file
 * @author (C) 2014 by Piotr Krysik <ptrkrysik@gmail.com>
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
#include <grgsm/gsmtap.h>
#include "check_msg_impl.h"

#define DATA_BYTES 23

namespace gr {
  namespace gsm {

  check_msg::sptr
  check_msg::make()
    {
      return gnuradio::get_initial_sptr
        (new check_msg_impl());
    }

    /*
     * Constructor
     */
  check_msg_impl::check_msg_impl()
      : gr::block("check_msg",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_in(pmt::mp("msg_in"));
      message_port_register_out(pmt::mp("msg_out"));
      set_msg_handler(pmt::mp("msg_in"),
       boost::bind(&check_msg_impl::print_msg, this, _1));
    }
  check_msg_impl::~check_msg_impl()
    {
    }
  void
  check_msg_impl::print_msg(pmt::pmt_t msg)
  {
	  printf("hello \n");
  }

  } /* namespace gsm */
} /* namespace gr */

