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

#ifndef INCLUDED_GRGSM_EXIT_CHANNEL_IMPL_H
#define INCLUDED_GRGSM_EXIT_CHANNEL_IMPL_H

#include <stddef.h>

#include <grgsm/gsmtap.h>
#include <grgsm/receiver/exit_channel.h>
#include <sqlite3.h>
#include <iomanip>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "grd_config_impl.h"
#include <fstream>
#include <sstream>

namespace gr {
  namespace gsm {

    class exit_channel_impl : public exit_channel
    {
     private:
    	int d_number_thread;
    	int d_i;
     public:
    	exit_channel_impl(int number_thread);
    	~exit_channel_impl();
        void handle_msg(pmt::pmt_t msg);
        int general_work(int noutput_items,
        	       gr_vector_int &ninput_items,
        	       gr_vector_const_void_star &input_items,
        	       gr_vector_void_star &output_items);
    };
  } // namespace gsm
} // namespace gr

#endif /* INCLUDED_GRGSM_TRX_BURST_IF_IMPL_H */
