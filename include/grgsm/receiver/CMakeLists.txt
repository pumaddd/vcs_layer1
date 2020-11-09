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

########################################################################
# Install public header files
########################################################################
install(FILES
    clock_offset_control.h
    cx_channel_hopper.h
    decodeC0.h
    super_block.h
    hopping_frequency.h
    gsm_decimator_ccf.h
    control_arb_resampler_ccf.h
    gsm_control_decimator.h
    file_source.h
    file_source_C0.h
    file_sink_base.h
    file_sink.h
    insert_db.h
    decode_cx.h
    rotator_cx.h
    sync.h
    grd_config.h
    check_msg.h
    pfb_channelizer_scanner.h
    exit_channel.h
    receiver.h DESTINATION include/grgsm/receiver
)
