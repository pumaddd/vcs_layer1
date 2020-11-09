# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

install(FILES
    gsm_input.xml
    gsm_receiver.xml
    gsm_receiver_with_uplink.xml
    gsm_fcch_burst_tagger.xml
    gsm_sch_detector.xml
    gsm_fcch_detector.xml
    gsm_cx_channel_hopper.xml
    gsm_receiver_modify.xml
    gsm_decodeC0.xml
    gsm_super_block.xml
    gsm_hopping_frequency_impl.xml
    gsm_decimator_ccf.xml
    gsm_pfb_arb_resampler_ccf.xml
    gsm_control_decimator.xml
    gsm_file_source.xml
    gsm_file_sink.xml
    gsm_msg_block.xml
    gsm_insert_db.xml
    gsm_file_source_C0.xml
    gsm_decode_cx.xml
    gsm_rotator_cx.xml
    gsm_sync.xml
    gsm_check_msg.xml
    multichannel_file_source.xml
    pfb_channelizer_test.xml
    gsm_decode_cx_uplink.xml
    gsm_rotator_cx_uplink.xml
    gsm_exit_channel.xml
    gsm_clock_offset_control.xml DESTINATION share/gnuradio/grc/blocks
)
