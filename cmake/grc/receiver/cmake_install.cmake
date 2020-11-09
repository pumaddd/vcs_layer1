# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver

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
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_input.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_receiver.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_receiver_with_uplink.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_fcch_burst_tagger.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_sch_detector.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_fcch_detector.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_cx_channel_hopper.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_receiver_modify.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_decodeC0.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_super_block.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_hopping_frequency_impl.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_decimator_ccf.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_file_source.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_file_sink.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_msg_block.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_insert_db.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_file_source_C0.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_decode_cx.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_rotator_cx.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_sync.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/receiver/gsm_clock_offset_control.xml"
    )
endif()

