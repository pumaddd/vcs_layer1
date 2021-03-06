# Copyright 2011 Free Software Foundation, Inc.
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
# Include python install macros
########################################################################
include(GrPython)
if(NOT PYTHONINTERP_FOUND)
    return()
endif()

########################################################################
# Install python sources
########################################################################
add_subdirectory(misc_utils)
add_subdirectory(receiver)
add_subdirectory(demapping)
add_subdirectory(transmitter)
add_subdirectory(trx)

GR_PYTHON_INSTALL(
    FILES
    __init__.py DESTINATION ${GR_PYTHON_DIR}/grgsm
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

set(GR_TEST_TARGET_DEPS gr-gsm)
set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/swig:${CMAKE_BINARY_DIR}/python/misc_utils)
GR_ADD_TEST(qa_arfcn ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_arfcn.py)
GR_ADD_TEST(qa_decryption ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_decryption.py)
GR_ADD_TEST(qa_burst_printer ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_burst_printer.py)
GR_ADD_TEST(qa_message_printer ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_message_printer.py)
GR_ADD_TEST(qa_burst_file_source ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_burst_file_source.py)
GR_ADD_TEST(qa_burst_timeslot_splitter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_burst_timeslot_splitter.py)
GR_ADD_TEST(qa_burst_sdcch_subslot_splitter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_burst_sdcch_subslot_splitter.py)
GR_ADD_TEST(qa_burst_timeslot_filter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_burst_timeslot_filter.py)
GR_ADD_TEST(qa_burst_sdcch_subslot_filter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_burst_sdcch_subslot_filter.py)
GR_ADD_TEST(qa_burst_fnr_filter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_burst_fnr_filter.py)
GR_ADD_TEST(qa_dummy_burst_filter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_dummy_burst_filter.py)
GR_ADD_TEST(qa_tch_f_decoder ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tch_f_decoder.py)
GR_ADD_TEST(qa_tch_h_decoder ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tch_h_decoder.py)
GR_ADD_TEST(qa_tch_f_chans_demapper ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tch_f_chans_demapper.py)
GR_ADD_TEST(qa_tch_h_chans_demapper ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_tch_h_chans_demapper.py)
#GR_ADD_TEST(qa_msg_to_tag ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_msg_to_tag.py)
#GR_ADD_TEST(qa_controlled_fractional_resampler_cc ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_controlled_fractional_resampler_cc.py)
#GR_ADD_TEST(qa_uplink_downlink_splitter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_uplink_downlink_splitter.py)
GR_ADD_TEST(qa_txtime_setter ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_txtime_setter.py)
