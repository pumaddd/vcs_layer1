# - Try to find Mysql-Connector-C++
# Once done, this will define
#
#  MYSQLCONNECTORCPP_FOUND - system has Mysql-Connector-C++ installed
#  MYSQLCONNECTORCPP_INCLUDE_DIRS - the Mysql-Connector-C++ include directories
#  MYSQLCONNECTORCPP_LIBRARIES - link these to use Mysql-Connector-C++
#
# The user may wish to set, in the CMake GUI or otherwise, this variable:
#  MYSQLCONNECTORCPP_ROOT_DIR - path to start searching for the module
find_path(MYSQLCONNECTORCPP_INCLUDE_DIR
	mysql_connection.h
	HINTS
	${MYSQLCONNECTORCPP_ROOT_DIR}
	PATH_SUFFIXES
	include)
		
find_library(MYSQLCONNECTORCPP_LIBRARY
	NAMES
	mysqlcppconn
	mysqlcppconn-static
	HINTS
	${MYSQLCONNECTORCPP_ROOT_DIR}
	PATH_SUFFIXES
	lib64
	lib)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MysqlConnectorCpp
	DEFAULT_MSG
	MYSQLCONNECTORCPP_INCLUDE_DIR
	MYSQLCONNECTORCPP_LIBRARY)
mark_as_advanced(MYSQLCONNECTORCPP_INCLUDE_DIRS MYSQLCONNECTORCPP_LIBRARIES)
if(MYSQLCONNECTORCPP_FOUND)
	message(DEBUG "\t set mysql ${MYSQLCONNECTORCPP_INCLUDE_DIR} ${MYSQLCONNECTORCPP_LIBRARY}")
	set(MYSQLCONNECTORCPP_INCLUDE_DIRS "${MYSQLCONNECTORCPP_INCLUDE_DIR}") # Add any dependencies here
	set(MYSQLCONNECTORCPP_LIBRARIES "${MYSQLCONNECTORCPP_LIBRARY}") # Add any dependencies here
endif()

