# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control

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
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control/gsm_burst_timeslot_splitter.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control/gsm_burst_fnr_filter.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control/gsm_burst_timeslot_filter.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control/gsm_burst_type_filter.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control/gsm_dummy_burst_filter.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control/gsm_burst_sdcch_subslot_splitter.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control/gsm_burst_sdcch_subslot_filter.xml"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/flow_control/gsm_uplink_downlink_splitter.xml"
    )
endif()

