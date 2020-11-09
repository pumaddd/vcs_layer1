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
#ifndef INCLUDED_GSM_DECODEC0_H
#define INCLUDED_GSM_DECODEC0_H
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
    class GRGSM_API decodeC0 : virtual public block
    {
     public:
      typedef boost::shared_ptr<decodeC0> sptr;
      /*!
       * \brief Return a shared_ptr to a new instance of gsm::receiver_modify.
       *
       * To avoid accidental use of raw pointers, gsm::receiver_modify's
       * constructor is in a private implementation
       * class. gsm::receiver_modify::make is the public interface for
       * creating new instances.
       */
      static sptr make(int osr, const std::vector<int> &cell_allocation, double fc, float fcO, double sample_rate,
    		  	  	  uint64_t sample_offset, std::string table_name, uint64_t index, bool process_uplink=false);
      
      virtual void set_cell_allocation(const std::vector<int> &cell_allocation) = 0;
      virtual void set_tseq_nums(const std::vector<int> & tseq_nums) = 0;
      virtual void reset() = 0;
      virtual void set_fc(float fc) = 0;
      virtual void set_fc0(float fcO) = 0;
      virtual void set_sample_rate(float sample_rate) = 0;
      virtual void set_cell_lac(int cell_id, int lac_id) = 0;
    };
   
  } // namespace gsm
} // namespace gr
#endif /* INCLUDED_GSM_DECODEC0_H */
