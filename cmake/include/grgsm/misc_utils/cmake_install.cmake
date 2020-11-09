# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/grgsm/misc_utils" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/bursts_printer.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/burst_file_source.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/burst_file_sink.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/collect_system_info.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/message_file_sink.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/message_file_source.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/extract_system_info.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/extract_immediate_assignment.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/extract_cmc.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/extract_assignment_cmd.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/controlled_rotator_cc.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/message_printer.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/tmsi_dumper.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/msg_to_tag.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/burst_to_fn_time.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/controlled_fractional_resampler_cc.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/time_spec.h"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/include/grgsm/misc_utils/fn_time.h"
    )
endif()

