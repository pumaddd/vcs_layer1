# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/receiver

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
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/receiver/gsm_input.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/receiver/fcch_burst_tagger.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/receiver/sch_detector.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/receiver/fcch_detector.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/receiver/decimator_ccf.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/receiver/msg_block.py"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/python/receiver/chirpz.py"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python2.7/dist-packages/grgsm" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/gsm_input.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/fcch_burst_tagger.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/sch_detector.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/fcch_detector.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/decimator_ccf.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/msg_block.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/chirpz.pyc"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/gsm_input.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/fcch_burst_tagger.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/sch_detector.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/fcch_detector.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/decimator_ccf.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/msg_block.pyo"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/python/receiver/chirpz.pyo"
    )
endif()

