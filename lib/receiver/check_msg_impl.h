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

#ifndef INCLUDED_GSM_CHECK_MSG_IMPL_H
#define INCLUDED_GSM_CHECK_MSG_IMPL_H

#include <grgsm/receiver/check_msg.h>

namespace gr {
  namespace gsm {

    class check_msg_impl : public check_msg
    {
     private:

      void print_msg(pmt::pmt_t msg);
     public:
      	 check_msg_impl();
      	 ~check_msg_impl();
    };

  } // namespace gsm
} // namespace gr

#endif /* INCLUDED_GSM_CONTROL_CHANNELS_DECODER_IMPL_H */

