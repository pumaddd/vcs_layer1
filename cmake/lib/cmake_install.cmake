# Install script for directory: /home/datqm/Desktop/new_source/GSM-Radio-Decryptor/lib

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
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgrgsm-0.42.2..so.0.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgrgsm-0.42.2..so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/libgrgsm-0.42.2..so.0.0.0"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/libgrgsm-0.42.2..so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgrgsm-0.42.2..so.0.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgrgsm-0.42.2..so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/usr/local/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/libgrgsm.so"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/libgrgsm-0.42.2..so.0"
    )
endif()

if("${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgrgsm-0.42.2..so.0.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgrgsm-0.42.2..so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/libgrgsm-0.42.2..so.0.0.0"
    "/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/libgrgsm-0.42.2..so"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgrgsm-0.42.2..so.0.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libgrgsm-0.42.2..so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/usr/local/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/decoding/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/decryption/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/demapping/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/flow_control/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/misc_utils/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/qa_utils/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/receiver/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/transmitter/cmake_install.cmake")
  include("/home/datqm/Desktop/new_source/GSM-Radio-Decryptor/cmake/lib/trx/cmake_install.cmake")

endif()

