/* -*- c++ -*- */
/* @file
 * @author (C) 2015 by Roman Khassraf <rkhassraf@gmail.com>
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
#include "burst_file_source_impl.h"
#include "stdio.h"
#include <grgsm/gsmtap.h>
#include <grgsm/gsm_constants.h>

#define SAMPLERATE_OUT (1625000.0/6.0 * 2)

namespace gr {
  namespace gsm {

    burst_file_source::sptr
    burst_file_source::make(const std::string &filename, uint64_t offset_byte, float samp_rate)
    {
      return gnuradio::get_initial_sptr
        (new burst_file_source_impl(filename, offset_byte, samp_rate));
    }

    /*
     * The private constructor
     */
    burst_file_source_impl::burst_file_source_impl(const std::string &filename, uint64_t offset_byte, float samp_rate)
      : gr::block("burst_file_source",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)),
              d_input_file(filename.c_str(), std::ifstream::binary),
              d_finished(false)
    {
    	d_offset_byte = 174 * 8 * floor(offset_byte/(samp_rate * 312.5/SAMPLERATE_OUT));
    	d_samp_rate = samp_rate;
    	d_input_file.seekg(d_offset_byte);
        message_port_register_out(pmt::mp("out"));
    }

    /*
     * Our virtual destructor.
     */
    burst_file_source_impl::~burst_file_source_impl()
    {
        if (d_finished == false){
            d_finished = true;
        }
    }

    bool burst_file_source_impl::start()
    {
        d_finished = false;
        d_thread = boost::shared_ptr<gr::thread::thread>
            (new gr::thread::thread(boost::bind(&burst_file_source_impl::run, this)));
        return block::start();
    }

    bool burst_file_source_impl::stop()
    {
        d_finished = true;
        d_thread->interrupt();
        d_thread->join();
        return block::stop();
    }

    bool burst_file_source_impl::finished()
    {
        return d_finished;
    }

    void burst_file_source_impl::run()
    {
        std::filebuf* pbuf = d_input_file.rdbuf();
        while (!d_finished)
        {
            pmt::pmt_t burst = pmt::deserialize(*pbuf);
            if (pmt::is_eof_object(burst)) {
                pmt::pmt_t pdu_header = pmt::make_dict();
                pdu_header = dict_add(pdu_header, pmt::string_to_symbol("Read"), pmt::string_to_symbol("Done"));
                uint8_t buf[sizeof(gsmtap_hdr) + BURST_SIZE];
                memset(buf, 0, sizeof(gsmtap_hdr) + BURST_SIZE);
                pmt::pmt_t blob = pmt::make_blob(buf, sizeof(gsmtap_hdr) + BURST_SIZE);
                pmt::pmt_t msg = pmt::cons(pdu_header, blob);
                message_port_pub(pmt::mp("out"), msg);
                break;
            }
          message_port_pub(pmt::mp("out"), burst);
        }
        d_input_file.close();
        post(pmt::mp("system"), pmt::cons(pmt::mp("done"), pmt::from_long(1)));
    }
  } /* namespace gsm */
} /* namespace gr */

