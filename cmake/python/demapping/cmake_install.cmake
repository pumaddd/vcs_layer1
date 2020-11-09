# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/demapping

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
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/demapping/gsm_bcch_ccch_demapper.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/demapping/gsm_bcch_ccch_sdcch4_demapper.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/demapping/gsm_sdcch8_demapper.py"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/grgsm" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/demapping/gsm_bcch_ccch_demapper.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/demapping/gsm_bcch_ccch_sdcch4_demapper.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/demapping/gsm_sdcch8_demapper.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/demapping/gsm_bcch_ccch_demapper.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/demapping/gsm_bcch_ccch_sdcch4_demapper.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/demapping/gsm_sdcch8_demapper.pyo"
    )
endif()

