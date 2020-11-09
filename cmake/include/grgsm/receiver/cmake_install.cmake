# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/grgsm/receiver" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/clock_offset_control.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/cx_channel_hopper.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/decodeC0.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/super_block.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/hopping_frequency.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/gsm_decimator_ccf.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/file_source.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/file_source_C0.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/file_sink_base.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/file_sink.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/insert_db.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/decode_cx.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/rotator_cx.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/sync.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/grd_config.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/receiver/receiver.h"
    )
endif()

