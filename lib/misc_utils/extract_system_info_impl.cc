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
//#include <unistd.h>
#include <map>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <grgsm/endian.h>
#include <boost/foreach.hpp>
extern "C" {
    #include <osmocom/gsm/gsm48_ie.h>
}


#include "extract_system_info_impl.h"

namespace gr {
  namespace gsm {
    boost::mutex extract_mutex;
    void extract_system_info_impl::process_bursts(pmt::pmt_t msg)
    {
        pmt::pmt_t burst_plus_header_blob = pmt::cdr(msg);
        gsmtap_hdr * header = (gsmtap_hdr *)pmt::blob_data(burst_plus_header_blob);
        chan_info info;
        info.id = be16toh(header->arfcn);
        info.pwr_db = header->signal_dbm;
        info.snr = header->snr_db;

        boost::mutex::scoped_lock lock(extract_mutex);
        
        if(d_c0_channels.find(info.id) != d_c0_channels.end()){
            d_c0_channels[info.id].copy_nonzero_elements(info);
        } else {
            d_c0_channels[info.id] = info;
        }
    }
    
    void extract_system_info_impl::process_sysinfo(pmt::pmt_t msg){
        pmt::pmt_t message_plus_header_blob = pmt::cdr(msg);
        uint8_t * message_plus_header = (uint8_t *)pmt::blob_data(message_plus_header_blob);
        gsmtap_hdr * header = (gsmtap_hdr *)message_plus_header;
        uint8_t * msg_elements = (uint8_t *)(message_plus_header+sizeof(gsmtap_hdr));
        struct gsm_sysinfo_freq freq[1024];
        chan_info info;

        if(msg_elements[2]==0x1b){ //System Infomation Type 3
            info.id = be16toh(header->arfcn);                            //take arfcn
            info.cell_id = (msg_elements[3]<<8)+msg_elements[4];         //take cell id
            info.lac = (msg_elements[8]<<8)+msg_elements[9];             //take lac
            info.mcc =  ((msg_elements[5] & 0xF)  * 100) + (((msg_elements[5] & 0xF0) >> 4) * 10) + ((msg_elements[6] & 0xF)); // take mcc
            info.mnc = (msg_elements[7] & 0xF) * 10 + (msg_elements[7]>>4); //take mnc

            /* Check Optional Selection Parameters exist */
            if (msg_elements[19] & 0x80) {
                info.cbq = (msg_elements[19] & (1 << 7-1)) != 0; // Get bit 7th from element 19th
                info.cell_reselect_offset = (msg_elements[19]) & 63; // Get 6 last bits from element 19th
                info.temporary_offset = msg_elements[20] >> 5; // Get 3 first bits from element 20th
                info.penalty_time = msg_elements[20] & 31; // Get 5 last bits form element 20th
            }
            info.mask = 2;

            if (((msg_elements[6] & 0xF0) >> 4) < 10) // we have a 3 digit mnc, see figure 10.5.3 of 3GPP TS 24.008
            {
                info.mnc *= 10;
                info.mnc += (msg_elements[6] & 0xF0) >> 4;
            }

            info.ccch_conf = (msg_elements[10] & 0x7); // ccch_conf
            
            boost::mutex::scoped_lock lock(extract_mutex);
            if(d_c0_channels.find(info.id) != d_c0_channels.end()){
                d_c0_channels[info.id].copy_nonzero_elements(info);
            } else {
                d_c0_channels[info.id] = info;
            }
        }
        else if(msg_elements[2]==0x1c){ //System Infomation Type 4
            info.id = be16toh(header->arfcn);                            //take arfcn
            info.lac = (msg_elements[6]<<8)+msg_elements[7];            //take lac
            info.mcc =  ((msg_elements[3] & 0xF) * 100) + (((msg_elements[3] & 0xF0) >> 4) * 10) + ((msg_elements[4] & 0xF)); // take mcc
            info.mnc = (msg_elements[5] & 0xF) * 10 + (msg_elements[5]>>4); //take mnc

            /* Check Optional Selection Parameters exist */
            if (msg_elements[20] & 0x80) {
                info.cbq = (msg_elements[20] & (1 << 7-1)) != 0; // Get bit 7th from element 20th
                info.cell_reselect_offset = (msg_elements[20]) & 63; // Get 6 last bits from element 20th
                info.temporary_offset = msg_elements[21] >> 5; // Get 3 first bits from element 21th
                info.penalty_time = msg_elements[21] & 31; // Get 5 last bits form element 21th
            }
            info.mask = 1;

            if (((msg_elements[4] & 0xF0) >> 4) < 10) // we have a 3 digit mnc, see figure 10.5.3 of 3GPP TS 24.008
            {
                info.mnc *= 10;
                info.mnc += (msg_elements[4] & 0xF0) >> 4;
            }
            
            boost::mutex::scoped_lock lock(extract_mutex);
            if(d_c0_channels.find(info.id) != d_c0_channels.end()){
                d_c0_channels[info.id].copy_nonzero_elements(info);
            } else {
                d_c0_channels[info.id] = info;
            }
        } 
        else if(msg_elements[2]==0x1a){ //System Information Type 2
            memset(freq, 0, sizeof(freq));
            info.id = be16toh(header->arfcn);                            //take arfcn
            boost::mutex::scoped_lock lock(extract_mutex);
            //read neighbour cells
            gsm48_decode_freq_list(freq, &msg_elements[3], 16, 0xce, 0x01);
            info.mask = 8;
            
            if(d_c0_channels.find(info.id) != d_c0_channels.end()){
                d_c0_channels[info.id].copy_nonzero_elements(info);
            } else {
                d_c0_channels[info.id] = info;
            }
            
            for(int arfcn=0; arfcn<sizeof(freq); arfcn++){
                if(freq[arfcn].mask==0x01){
                    d_c0_channels[info.id].neighbour_cells.insert(arfcn);
                }
            }
        }
        else if(msg_elements[2]==0x02){ //System Information Type 2bis
            memset(freq, 0, sizeof(freq));
            info.id = be16toh(header->arfcn);                            //take arfcn
            boost::mutex::scoped_lock lock(extract_mutex);
            //read neighbour cells
            gsm48_decode_freq_list(freq, &msg_elements[3], 16, 0xce, 0x01);
            if(d_c0_channels.find(info.id) != d_c0_channels.end()){
                d_c0_channels[info.id].copy_nonzero_elements(info);
            } else {
                d_c0_channels[info.id] = info;
            }
            
            for(int arfcn=0; arfcn<sizeof(freq); arfcn++){
                if(freq[arfcn].mask==0x01){
                    d_c0_channels[info.id].neighbour_cells.insert(arfcn);
                }
            }
        }
        else if(msg_elements[2]==0x03){ //System Information Type 2ter
            memset(freq, 0, sizeof(freq));
            info.id = be16toh(header->arfcn);                            //take arfcn
            boost::mutex::scoped_lock lock(extract_mutex);
            //read neighbour cells
            gsm48_decode_freq_list(freq, &msg_elements[3], 16, 0x8e, 0x01);
            info.mask = 4;
            if(d_c0_channels.find(info.id) != d_c0_channels.end()){
                d_c0_channels[info.id].copy_nonzero_elements(info);
            } else {
                d_c0_channels[info.id] = info;
            }
            
            for(int arfcn=0; arfcn<sizeof(freq); arfcn++){
                if(freq[arfcn].mask==0x01){
                    d_c0_channels[info.id].neighbour_cells.insert(arfcn);
                }
            }
        }
        else if(msg_elements[2]==0x19){ //System Information Type 1
            memset(freq, 0, sizeof(freq));
            info.id = be16toh(header->arfcn);                            //take arfcn
            boost::mutex::scoped_lock lock(extract_mutex);
            //read cell arfcn's
            gsm48_decode_freq_list(freq, &msg_elements[3], 16, 0x8e, 0x01);
            info.mask = 16;
            if(d_c0_channels.find(info.id) != d_c0_channels.end()){
                d_c0_channels[info.id].copy_nonzero_elements(info);
            } else {
                d_c0_channels[info.id] = info;
            }
            
            for(int arfcn=0; arfcn<sizeof(freq); arfcn++){
                if(freq[arfcn].mask==0x01){
                    d_c0_channels[info.id].cell_arfcns.insert(arfcn);
                }
            }
        }
    else { 
        /* Not System Infomation, no need handle */
    }
    }
    
    std::vector<int> extract_system_info_impl::get_chans()
    {
        std::vector<int> chans_ids;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            chans_ids.push_back(i.second.id);
        }
        return chans_ids;
    }
    
    std::vector<int> extract_system_info_impl::get_lac()
    {
        std::vector<int> lacs;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            lacs.push_back(i.second.lac);
        }
        return lacs;
    }
    
    std::vector<int> extract_system_info_impl::get_mcc()
    {
        std::vector<int> mccs;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            mccs.push_back(i.second.mcc);
        }
        return mccs;
    }
    
    std::vector<int> extract_system_info_impl::get_mnc()
    {
        std::vector<int> mncs;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            mncs.push_back(i.second.mnc);
        }
        return mncs;
    }
    
    std::vector<int> extract_system_info_impl::get_cell_id()
    {
        std::vector<int> cell_ids;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            cell_ids.push_back(i.second.cell_id);
        }
        return cell_ids;
    }
    
    std::vector<int> extract_system_info_impl::get_pwrs()
    {
        std::vector<int> pwrs;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            pwrs.push_back(i.second.pwr_db);
        }
        return pwrs;
    }
    
    std::vector<int> extract_system_info_impl::get_ccch_conf()
    {
        std::vector<int> ccch_confs;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            ccch_confs.push_back(i.second.ccch_conf);
        }
        return ccch_confs;
    }

    std::vector<int> extract_system_info_impl::get_cbq()
    {
        std::vector<int> cbqs;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            cbqs.push_back(i.second.cbq);
        }
        return cbqs;
    }

    std::vector<int> extract_system_info_impl::get_cell_reselect_offset()
    {
        std::vector<int> cell_reselect_offsets;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            cell_reselect_offsets.push_back(i.second.cell_reselect_offset);
        }
        return cell_reselect_offsets;
    }

    std::vector<int> extract_system_info_impl::get_temporary_offset()
    {
        std::vector<int> temporary_offsets;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            temporary_offsets.push_back(i.second.temporary_offset);
        }
        return temporary_offsets;
    }

    std::vector<int> extract_system_info_impl::get_penalty_time()
    {
        std::vector<int> penalty_times;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            penalty_times.push_back(i.second.penalty_time);
        }
        return penalty_times;
    }

    std::vector<int> extract_system_info_impl::get_sum_pwr_db()
    {
        std::vector<int> sum_pwr_dbs;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            sum_pwr_dbs.push_back(i.second.sum_pwr_db);
        }
        return sum_pwr_dbs;
    }

    std::vector<int> extract_system_info_impl::get_sum_snr()
    {
        std::vector<int> sum_snrs;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            sum_snrs.push_back(i.second.sum_snr);
        }
        return sum_snrs;
    }

    std::vector<int> extract_system_info_impl::get_count_burst()
    {
        std::vector<int> count_bursts;
        BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
            count_bursts.push_back(i.second.count_burst);
        }
        return count_bursts;
    }
    
    std::vector<int> extract_system_info_impl::get_mask()
    {
    std::vector<int> masks;
    BOOST_FOREACH(chan_info_map::value_type &i, d_c0_channels){
        masks.push_back(i.second.mask);
    }
    return masks;
    }

    std::vector<int> extract_system_info_impl::get_neighbours(int chan_id)
    {
        std::vector<int> neighbour_cells;
        BOOST_FOREACH(int n, d_c0_channels[chan_id].neighbour_cells){
            neighbour_cells.push_back(n);
        }
        return neighbour_cells;
    }
        
    std::vector<int> extract_system_info_impl::get_cell_arfcns(int chan_id)
    {
        std::vector<int> cell_arfcns;
        BOOST_FOREACH(int n, d_c0_channels[chan_id].cell_arfcns){
            cell_arfcns.push_back(n);
        }
        return cell_arfcns;
    }
    
    void extract_system_info_impl::reset()
    {
        d_c0_channels.clear();
        if(!empty_p(pmt::mp("bursts"))){
//            delete_head_blocking(pmt::mp("bursts"));
        }
        if(!empty_p(pmt::mp("msgs"))){
//            delete_head_blocking(pmt::mp("msgs"));
        }
    }
    
    extract_system_info::sptr
    extract_system_info::make()
    {
      return gnuradio::get_initial_sptr
        (new extract_system_info_impl());
    }

    /*
     * The private constructor
     */
    extract_system_info_impl::extract_system_info_impl()
      : gr::block("extract_system_info",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0)), 
              after_reset(false)
    {
        message_port_register_in(pmt::mp("bursts"));
        set_msg_handler(pmt::mp("bursts"), boost::bind(&extract_system_info_impl::process_bursts, this, _1));
        message_port_register_in(pmt::mp("msgs"));
        set_msg_handler(pmt::mp("msgs"), boost::bind(&extract_system_info_impl::process_sysinfo, this, _1));
    }
    
    /*
     * Our virtual destructor.
     */
    extract_system_info_impl::~extract_system_info_impl()
    {
    }


  } /* namespace gsm */
} /* namespace gr */

