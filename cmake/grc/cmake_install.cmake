# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/grc/gsm_block_tree.xml")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/grc/decoding/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/grc/decryption/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/grc/demapping/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/grc/receiver/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/grc/flow_control/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/grc/misc_utils/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/grc/transmitter/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/grc/trx/cmake_install.cmake")

endif()

