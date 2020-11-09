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

add_sources(
    clock_offset_control_impl.cc
    cx_channel_hopper_impl.cc
    receiver_config.cc
    receiver_impl.cc
    viterbi_detector.cc
    time_sample_ref.cc
    receiver_modify_impl.cc
    decodeC0_impl.cc
    super_block_impl.cc
    hopping_frequency_impl.cc
    gsm_decimator_ccf_impl.cc
    control_arb_resampler_ccf_impl.cc
    gsm_control_decimator_impl.cc
    file_source_impl.cc
    file_source_impl_C0.cc
    file_sink_base.cc
    file_sink_impl.cc
    insert_db_impl.cc
    decode_cx_all.cc
    rotator_cx_impl.cc
    sync_impl.cc
    grd_config_impl.cc
    check_msg_impl.cc
    pfb_channelizer_scanner_impl.cc
    exit_channel_impl.cc
)
