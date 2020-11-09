# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/grgsm/trx" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/__init__.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/udp_link.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/ctrl_if.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/ctrl_if_bb.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/radio_if.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/radio_if_uhd.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/radio_if_lms.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/radio_if_grc.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/transceiver.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/trx/dict_toggle_sign.py"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/grgsm/trx" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/__init__.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/udp_link.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/ctrl_if.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/ctrl_if_bb.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/radio_if.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/radio_if_uhd.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/radio_if_lms.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/radio_if_grc.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/transceiver.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/dict_toggle_sign.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/__init__.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/udp_link.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/ctrl_if.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/ctrl_if_bb.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/radio_if.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/radio_if_uhd.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/radio_if_lms.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/radio_if_grc.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/transceiver.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/trx/dict_toggle_sign.pyo"
    )
endif()

