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

#ifndef INCLUDED_GSM_EXTRACT_SYSTEM_INFO_IMPL_H
#define INCLUDED_GSM_EXTRACT_SYSTEM_INFO_IMPL_H

#include <grgsm/misc_utils/extract_system_info.h>
#include <set>
#include <map>
#include <vector>

namespace gr {
  namespace gsm {
    class chan_info {
      public:
        unsigned int id;
        int8_t pwr_db;
        unsigned int arfcn;
        unsigned int lac;
        unsigned int cell_id;
        unsigned int mcc;
        unsigned int mnc;
        unsigned int ccch_conf;
        std::set<int> neighbour_cells;
        std::set<int> cell_arfcns;
        int snr;
        unsigned int cbq;
        unsigned int cell_reselect_offset;
        unsigned int temporary_offset;
        unsigned int penalty_time;
        unsigned int sum_pwr_db;
        unsigned int sum_snr;
        unsigned int count_burst; 
        unsigned char mask;
        
        chan_info() :  id(-1), pwr_db(0), arfcn(0), lac(0), cell_id(0), mcc(0), mnc(0), ccch_conf(-1), snr(-1), cbq(-1), cell_reselect_offset(-1), temporary_offset(-1), penalty_time(-1), sum_pwr_db(0), sum_snr(0), count_burst(0), mask(0){}
        chan_info(const chan_info & info) : id(info.id), pwr_db(info.pwr_db), arfcn(info.arfcn), lac(info.lac), cell_id(info.cell_id), mcc(info.mcc), mnc(info.mnc), ccch_conf(info.ccch_conf), snr(info.snr), cbq(info.cbq), cell_reselect_offset(info.cell_reselect_offset), temporary_offset(info.temporary_offset), penalty_time(info.penalty_time), sum_pwr_db(info.sum_pwr_db), sum_snr(info.sum_snr), count_burst(info.count_burst), mask(info.mask){}
        ~chan_info(){}
        void copy_nonzero_elements(const chan_info & info){
            id = info.id;
            pwr_db = info.pwr_db;
            arfcn = info.arfcn;
            lac = (info.lac!=0) ? info.lac : lac;
            cell_id = (info.cell_id!=0) ? info.cell_id : cell_id;
            mcc = (info.mcc!=0) ? info.mcc : mcc;
            mnc = (info.mnc!=0) ? info.mnc : mnc;
            ccch_conf = (info.ccch_conf!=-1) ? info.ccch_conf : ccch_conf;

            cbq = (info.cbq!=-1) ? info.cbq : cbq;
            cell_reselect_offset = (info.cell_reselect_offset!=-1) ? info.cell_reselect_offset : cell_reselect_offset;
            temporary_offset = (info.temporary_offset!=-1) ? info.temporary_offset : temporary_offset;
            penalty_time = (info.penalty_time!=-1) ? (info.penalty_time + 1)*20 : penalty_time;
            
            if (info.pwr_db < 0) {
              /* Burst */
              // get only burst snr and power to calculate chanel info
              snr = (info.snr > 0) ? info.snr : 255 + info.snr;
              sum_snr += snr;
              sum_pwr_db += pwr_db;
              count_burst += 1;
            } else {
              /* Packet, not handle */
            }
            mask = mask | info.mask;
        }
    };


    struct compare_id {
        inline bool operator()(const chan_info &a, const chan_info &b) const
        {
            return a.id < b.id;
        }
    };
    struct compare_pwr {
        inline bool operator()(const chan_info &a, const chan_info &b) const
        {
            return a.pwr_db < b.pwr_db;
        }
    };

    typedef std::map<unsigned int, chan_info> chan_info_map;
    class extract_system_info_impl : public extract_system_info
    {
     private:
      void process_bursts(pmt::pmt_t burst);
      void process_sysinfo(pmt::pmt_t msg);
      chan_info_map d_c0_channels;
      bool after_reset;
      void decode_neighbour_cells(uint8_t * data, unsigned int offset, unsigned int chan_id);
//      void dissect_channel_list_n_range(guint32 offset, guint len, gint range)
     public:
      virtual std::vector<int> get_chans();
      virtual std::vector<int> get_pwrs();
      virtual std::vector<int> get_lac();
      virtual std::vector<int> get_cell_id();
      virtual std::vector<int> get_mcc();
      virtual std::vector<int> get_mnc();
      virtual std::vector<int> get_ccch_conf();
      virtual std::vector<int> get_cell_arfcns(int chan_id);
      virtual std::vector<int> get_neighbours(int chan_id);
      virtual std::vector<int> get_cbq();
      virtual std::vector<int> get_cell_reselect_offset();
      virtual std::vector<int> get_temporary_offset();
      virtual std::vector<int> get_penalty_time();
      virtual std::vector<int> get_sum_pwr_db();
      virtual std::vector<int> get_sum_snr();
      virtual std::vector<int> get_count_burst();
      virtual std::vector<int> get_mask();
      virtual void reset();
      extract_system_info_impl();
      ~extract_system_info_impl();
    };
  } // namespace gsm
} // namespace gr

#endif /* INCLUDED_GSM_EXTRACT_SYSTEM_INFO_IMPL_H */

