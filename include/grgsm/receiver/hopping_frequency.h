/* -*- c++ -*- */
/*
 * @file
 * @author (C) 2014-2016 by Piotr Krysik <ptrkrysik@gmail.com>
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


#ifndef INCLUDED_GSM_HOPPING_FREQUENCY_H
#define INCLUDED_GSM_HOPPING_FREQUENCY_H

#include <grgsm/api.h>
#include <gnuradio/block.h>
#include <gnuradio/sync_block.h>
#include <vector>

namespace gr {
  namespace gsm {

    /*!
     * \brief <+description of block+>
     * \ingroup gsm
     *
     */
    class GRGSM_API hopping_frequency : virtual public block
    {
     public:
      typedef boost::shared_ptr<hopping_frequency> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of gsm::receiver.
       *
       * To avoid accidental use of raw pointers, gsm::receiver's
       * constructor is in a private implementation
       * class. gsm::receiver::make is the public interface for
       * creating new instances.
       */
      virtual void set_ma(std::vector<int> ma) = 0;
      virtual void set_fc(std::vector<double> fc) = 0;
      virtual void set_maio(int maio) = 0;
      virtual void set_hsn(int hsn) = 0;
      virtual void set_id_start(int id_start) = 0;
      virtual void set_sample_start(int sample_start) = 0;
      virtual void set_sample_rate(float set_sample_rate) = 0;
      virtual void set_bandwidth(float bandwidth) = 0;
      static sptr make(const std::vector<int> &cell_allocation, const std::vector<double> &fc, int maio, int hsn,
    		  	  	  float bd, double sample_rate, std::string table_name, uint64_t id_start, double samp_in, uint8_t time_slot, uint64_t index);
    };

  } // namespace gsm
} // namespace gr

#endif /* INCLUDED_GSM_SUPER_BLOCK_H */

