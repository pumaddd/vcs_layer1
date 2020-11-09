/* -*- c++ -*- */
/*
 * @file
 * @author (C) 2014-2017 by Piotr Krysik <ptrkrysik@gmail.com>
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


#define GRGSM_API

%include <std_pair.i>
%template(pairud) std::pair<unsigned long long,double>;

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "grgsm_swig_doc.i"

%{
#include "grgsm/constants.h"
#include "grgsm/receiver/receiver.h"
#include "grgsm/receiver/receiver_modify.h"
#include "grgsm/receiver/decodeC0.h"
#include "grgsm/receiver/clock_offset_control.h"
#include "grgsm/receiver/cx_channel_hopper.h"
#include "grgsm/receiver/super_block.h"
#include "grgsm/receiver/decode_cx.h"
#include "grgsm/receiver/rotator_cx.h"
#include "grgsm/receiver/hopping_frequency.h"
#include "grgsm/receiver/gsm_decimator_ccf.h"
#include "grgsm/receiver/control_arb_resampler_ccf.h"
#include "grgsm/receiver/gsm_control_decimator.h"
#include "grgsm/receiver/pfb_channelizer_scanner.h"
#include "grgsm/receiver/file_source.h"
#include "grgsm/receiver/file_source_C0.h"
#include "grgsm/receiver/file_sink.h"
#include "grgsm/receiver/file_sink_base.h"
#include "grgsm/receiver/insert_db.h"
#include "grgsm/receiver/sync.h"
#include "grgsm/receiver/grd_config.h"
#include "grgsm/receiver/exit_channel.h"
#include "grgsm/receiver/check_msg.h"

#include "grgsm/decoding/control_channels_decoder.h"
#include "grgsm/decoding/tch_f_decoder.h"
#include "grgsm/decoding/tch_h_decoder.h"
#include "grgsm/decryption/decryption.h"
#include "grgsm/demapping/universal_ctrl_chans_demapper.h"
#include "grgsm/demapping/tch_f_chans_demapper.h"
#include "grgsm/demapping/tch_h_chans_demapper.h"
#include "grgsm/flow_control/common.h"
#include "grgsm/flow_control/burst_timeslot_splitter.h"
#include "grgsm/flow_control/burst_sdcch_subslot_splitter.h"
#include "grgsm/flow_control/burst_timeslot_filter.h"
#include "grgsm/flow_control/burst_sdcch_subslot_filter.h"
#include "grgsm/flow_control/burst_fnr_filter.h"
#include "grgsm/flow_control/burst_type_filter.h"
#include "grgsm/flow_control/dummy_burst_filter.h"
#include "grgsm/flow_control/uplink_downlink_splitter.h"
#include "grgsm/misc_utils/bursts_printer.h"
#include "grgsm/misc_utils/controlled_rotator_cc.h"
#include "grgsm/misc_utils/extract_system_info.h"
#include "grgsm/misc_utils/extract_immediate_assignment.h"
#include "grgsm/misc_utils/message_printer.h"
#include "grgsm/misc_utils/tmsi_dumper.h"
#include "grgsm/misc_utils/burst_file_sink.h"
#include "grgsm/misc_utils/burst_file_source.h"
#include "grgsm/misc_utils/burst_file_source_fn.h"
#include "grgsm/misc_utils/collect_system_info.h"
#include "grgsm/misc_utils/extract_cmc.h"
#include "grgsm/misc_utils/extract_assignment_cmd.h"

#include "grgsm/qa_utils/burst_sink.h"
#include "grgsm/qa_utils/burst_source.h"
#include "grgsm/qa_utils/message_source.h"
#include "grgsm/qa_utils/message_sink.h"
#include "grgsm/misc_utils/message_file_sink.h"
#include "grgsm/misc_utils/message_file_source.h"
#include "grgsm/misc_utils/msg_to_tag.h"
#include "grgsm/misc_utils/controlled_fractional_resampler_cc.h"
#include "grgsm/misc_utils/burst_to_fn_time.h"
#include "grgsm/misc_utils/fn_time.h"
#include "grgsm/transmitter/txtime_setter.h"
#include "grgsm/transmitter/preprocess_tx_burst.h"
#include "grgsm/transmitter/gen_test_ab.h"
#include "grgsm/trx/trx_burst_if.h"
%}

%include "constants.i"

%include "grgsm/receiver/receiver.h"
GR_SWIG_BLOCK_MAGIC2(gsm, receiver);
%include "grgsm/receiver/receiver_modify.h"
GR_SWIG_BLOCK_MAGIC2(gsm, receiver_modify);
%include "grgsm/receiver/decodeC0.h"
GR_SWIG_BLOCK_MAGIC2(gsm, decodeC0);
%include "grgsm/receiver/clock_offset_control.h"
GR_SWIG_BLOCK_MAGIC2(gsm, clock_offset_control);
%include "grgsm/receiver/cx_channel_hopper.h"
GR_SWIG_BLOCK_MAGIC2(gsm, cx_channel_hopper);
%include "grgsm/receiver/super_block.h"
GR_SWIG_BLOCK_MAGIC2(gsm, super_block);
%include "grgsm/receiver/decode_cx.h"
GR_SWIG_BLOCK_MAGIC2(gsm, decode_cx);
%include "grgsm/receiver/rotator_cx.h"
GR_SWIG_BLOCK_MAGIC2(gsm, rotator_cx);
%include "grgsm/receiver/hopping_frequency.h"
GR_SWIG_BLOCK_MAGIC2(gsm, hopping_frequency);
%include "grgsm/receiver/pfb_channelizer_scanner.h"
GR_SWIG_BLOCK_MAGIC2(gsm, pfb_channelizer_scanner);
%include "grgsm/receiver/gsm_decimator_ccf.h"
GR_SWIG_BLOCK_MAGIC2(gsm, gsm_decimator_ccf);
%include "grgsm/receiver/control_arb_resampler_ccf.h"
GR_SWIG_BLOCK_MAGIC2(gsm, control_arb_resampler_ccf);
%include "grgsm/receiver/gsm_control_decimator.h"
GR_SWIG_BLOCK_MAGIC2(gsm, gsm_control_decimator);
%include "grgsm/receiver/file_source.h"
GR_SWIG_BLOCK_MAGIC2(gsm, file_source);
%include "grgsm/receiver/file_source_C0.h"
GR_SWIG_BLOCK_MAGIC2(gsm, file_source_C0);
%include "grgsm/receiver/file_sink.h"
%include "grgsm/receiver/file_sink_base.h"
GR_SWIG_BLOCK_MAGIC2(gsm, file_sink);
%include "grgsm/receiver/insert_db.h"
GR_SWIG_BLOCK_MAGIC2(gsm, insert_db);
%include "grgsm/receiver/sync.h"
GR_SWIG_BLOCK_MAGIC2(gsm, sync);
%include "grgsm/receiver/grd_config.h"
GR_SWIG_BLOCK_MAGIC2(gsm, grd_config);
%include "grgsm/receiver/exit_channel.h"
GR_SWIG_BLOCK_MAGIC2(gsm, exit_channel);
%include "grgsm/receiver/check_msg.h"
GR_SWIG_BLOCK_MAGIC2(gsm, check_msg);

%include "grgsm/decoding/control_channels_decoder.h"
GR_SWIG_BLOCK_MAGIC2(gsm, control_channels_decoder);
%include "grgsm/decoding/tch_f_decoder.h"
GR_SWIG_BLOCK_MAGIC2(gsm, tch_f_decoder);
%include "grgsm/decoding/tch_h_decoder.h"
GR_SWIG_BLOCK_MAGIC2(gsm, tch_h_decoder);

%include "grgsm/decryption/decryption.h"
GR_SWIG_BLOCK_MAGIC2(gsm, decryption);

%include "grgsm/demapping/universal_ctrl_chans_demapper.h"
GR_SWIG_BLOCK_MAGIC2(gsm, universal_ctrl_chans_demapper);
%include "grgsm/demapping/tch_f_chans_demapper.h"
GR_SWIG_BLOCK_MAGIC2(gsm, tch_f_chans_demapper);
%include "grgsm/demapping/tch_h_chans_demapper.h"
GR_SWIG_BLOCK_MAGIC2(gsm, tch_h_chans_demapper);

%include "grgsm/flow_control/common.h"
%include "grgsm/flow_control/burst_timeslot_splitter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_timeslot_splitter);
%include "grgsm/flow_control/burst_sdcch_subslot_splitter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_sdcch_subslot_splitter);
%include "grgsm/flow_control/burst_timeslot_filter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_timeslot_filter);
%include "grgsm/flow_control/burst_sdcch_subslot_filter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_sdcch_subslot_filter);
%include "grgsm/flow_control/burst_fnr_filter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_fnr_filter);
%include "grgsm/flow_control/burst_type_filter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_type_filter);
%include "grgsm/flow_control/dummy_burst_filter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, dummy_burst_filter);
%include "grgsm/flow_control/uplink_downlink_splitter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, uplink_downlink_splitter);


%include "grgsm/misc_utils/bursts_printer.h"
GR_SWIG_BLOCK_MAGIC2(gsm, bursts_printer);
%include "grgsm/misc_utils/burst_file_sink.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_file_sink);
%include "grgsm/misc_utils/burst_file_source.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_file_source);
%include "grgsm/misc_utils/burst_file_source_fn.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_file_source_fn);
%include "grgsm/misc_utils/collect_system_info.h"
GR_SWIG_BLOCK_MAGIC2(gsm, collect_system_info);
%include "grgsm/misc_utils/extract_system_info.h"
GR_SWIG_BLOCK_MAGIC2(gsm, extract_system_info);
%include "grgsm/misc_utils/extract_immediate_assignment.h"
GR_SWIG_BLOCK_MAGIC2(gsm, extract_immediate_assignment);
%include "grgsm/misc_utils/controlled_rotator_cc.h"
GR_SWIG_BLOCK_MAGIC2(gsm, controlled_rotator_cc);
%include "grgsm/misc_utils/message_printer.h"
GR_SWIG_BLOCK_MAGIC2(gsm, message_printer);
%include "grgsm/misc_utils/tmsi_dumper.h"
GR_SWIG_BLOCK_MAGIC2(gsm, tmsi_dumper);
%include "grgsm/misc_utils/message_file_sink.h"
GR_SWIG_BLOCK_MAGIC2(gsm, message_file_sink);
%include "grgsm/misc_utils/message_file_source.h"
GR_SWIG_BLOCK_MAGIC2(gsm, message_file_source);
%include "grgsm/misc_utils/msg_to_tag.h"
GR_SWIG_BLOCK_MAGIC2(gsm, msg_to_tag);
%include "grgsm/misc_utils/controlled_fractional_resampler_cc.h"
GR_SWIG_BLOCK_MAGIC2(gsm, controlled_fractional_resampler_cc);
%include "grgsm/misc_utils/extract_cmc.h"
GR_SWIG_BLOCK_MAGIC2(gsm, extract_cmc);
%include "grgsm/misc_utils/extract_assignment_cmd.h"
GR_SWIG_BLOCK_MAGIC2(gsm, extract_assignment_cmd);
%include "grgsm/trx/trx_burst_if.h"
GR_SWIG_BLOCK_MAGIC2(gsm, trx_burst_if);
%include "grgsm/misc_utils/burst_to_fn_time.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_to_fn_time);

%include "grgsm/qa_utils/burst_sink.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_sink);
%include "grgsm/qa_utils/burst_source.h"
GR_SWIG_BLOCK_MAGIC2(gsm, burst_source);
%include "grgsm/qa_utils/message_source.h"
GR_SWIG_BLOCK_MAGIC2(gsm, message_source);
%include "grgsm/qa_utils/message_sink.h"
GR_SWIG_BLOCK_MAGIC2(gsm, message_sink);

%include "grgsm/misc_utils/fn_time.h"

%include "grgsm/transmitter/txtime_setter.h"
GR_SWIG_BLOCK_MAGIC2(gsm, txtime_setter);
%include "grgsm/transmitter/preprocess_tx_burst.h"
GR_SWIG_BLOCK_MAGIC2(gsm, preprocess_tx_burst);
%include "grgsm/transmitter/gen_test_ab.h"
GR_SWIG_BLOCK_MAGIC2(gsm, gen_test_ab);
