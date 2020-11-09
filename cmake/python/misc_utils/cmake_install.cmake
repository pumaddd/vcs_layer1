# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/misc_utils

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/grgsm" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/misc_utils/arfcn.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/misc_utils/clock_offset_corrector_tagged.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/misc_utils/hier_block.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/misc_utils/fn_time.py"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/grgsm" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils/arfcn.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils/clock_offset_corrector_tagged.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils/hier_block.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils/fn_time.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils/arfcn.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils/clock_offset_corrector_tagged.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils/hier_block.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/misc_utils/fn_time.pyo"
    )
endif()

