# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_extract_system_info.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_extract_immediate_assignment.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_collect_system_info.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_extract_cmc.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_extract_assignment_cmd.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_controlled_rotator_cc.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_message_printer.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_bursts_printer.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_clock_offset_corrector_tagged.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_tmsi_dumper.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_burst_file_sink.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_burst_file_source.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_message_file_sink.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_message_file_source.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_msg_to_tag.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_controlled_fractional_resampler_cc.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/misc_utils/gsm_burst_to_fn_time.xml"
    )
endif()

