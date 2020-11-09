# Copyright (C) 2007-2009 LuaDist.
# Created by Peter Kapec <kapecp@gmail.com>
# Redistribution and use of this file is allowed according to the terms of the MIT license.
# For details see the COPYRIGHT file distributed with LuaDist.
#	Note:
#		Searching headers and libraries is very simple and is NOT as powerful as scripts
#		distributed with CMake, because LuaDist defines directories to search for.
#		Everyone is encouraged to contact the author with improvements. Maybe this file
#		becomes part of CMake distribution sometimes.
# Look for the header file.
FIND_PATH(LIBAIO_INCLUDE_DIR NAMES libaio.h)

# Look for the library.
FIND_LIBRARY(LIBAIO_LIBRARY NAMES libaio.so)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBAIO DEFAULT_MSG LIBAIO_LIBRARY LIBAIO_INCLUDE_DIR)

# Copy the results to the output variables.
IF(LIBAIO_FOUND)
    message(DEBUG "\t set aio ${LIBAIO_LIBRARY} ${LIBAIO_INCLUDE_DIR}")
	SET(LIBAIO_LIBRARIES ${LIBAIO_LIBRARY})
	SET(LIBAIO_INCLUDE_DIRS ${LIBAIO_INCLUDE_DIR})
ELSE(LIBAIO_FOUND)
	SET(LIBAIO_LIBRARIES)
	SET(LIBAIO_INCLUDE_DIRS)
ENDIF(LIBAIO_FOUND)

MARK_AS_ADVANCED(LIBAIO_INCLUDE_DIRS LIBAIO_LIBRARIES)
