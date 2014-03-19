# Copyright 2010-2013 David "Alemarius Nexus" Lerch
# 
# This file is part of nxcommon.
#
# nxcommon is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# nxcommon is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with nxcommon.  If not, see <http://www.gnu.org/licenses/>.

# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(nxcommon C CXX)

INCLUDE(TestBigEndian)

ADD_DEFINITIONS(-D__STDC_LIMIT_MACROS -DUCHAR_TYPE=char16_t)

TEST_BIG_ENDIAN(NXCOMMON_BIG_ENDIAN)


SET(NXCOMMON_EXCEPTION_POSITION_INFO OFF CACHE BOOL "If true, exception messages will include the full path to the source file and the line in which the exception was thrown.")
SET(NXCOMMON_SQLITE_ENABLED OFF CACHE BOOL "Enable SQLite support for the nxcommon SQL module. Uses builtin SQLite code.")
SET(NXCOMMON_MYSQL_ENABLED OFF CACHE BOOL "Enable MySQL support for the nxcommon SQL module. Requires libmysqlclient.")

IF(NXCOMMON_EXCEPTION_POSITION_INFO)
    MESSAGE(STATUS "Exception position information enabled.")
ELSE(NXCOMMON_EXCEPTION_POSITION_INFO)
    MESSAGE(STATUS "Exception position information disabled.")
ENDIF(NXCOMMON_EXCEPTION_POSITION_INFO)

IF(NXCOMMON_SQLITE_ENABLED)
    MESSAGE(STATUS "SQLite support enabled.")
ELSE(NXCOMMON_SQLITE_ENABLED)
    MESSAGE(STATUS "SQLite support disabled.")
ENDIF(NXCOMMON_SQLITE_ENABLED)

IF(NXCOMMON_MYSQL_ENABLED)
    MESSAGE(STATUS "MySQL support enabled.")
ELSE(NXCOMMON_MYSQL_ENABLED)
    MESSAGE(STATUS "MySQL support disabled.")
ENDIF(NXCOMMON_MYSQL_ENABLED)


FIND_PATH(ICU_INCLUDE_PATH unicode/utypes.h)
FIND_LIBRARY(ICU_BASE_LIB_PATH NAMES icuuc libicuuc)
FIND_LIBRARY(ICU_DATA_LIB_PATH NAMES icudata libicuudata)

IF(NXCOMMON_MYSQL_ENABLED)
    FIND_LIBRARY(LIBMYSQLCLIENT_LIB_PATH NAMES mysqlclient libmysqlclient mysqlclient18 libmysqlclient18)
    FIND_PATH(LIBMYSQLCLIENT_INCLUDE_DIR mysql/mysql.h)
ENDIF(NXCOMMON_MYSQL_ENABLED)


SET(LIBRARIES "")
SET(INCLUDES "")

IF(NXCOMMON_MYSQL_ENABLED)
    SET(LIBRARIES ${LIBRARIES} "${LIBMYSQLCLIENT_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${LIBMYSQLCLIENT_INCLUDE_DIR}")
ENDIF(NXCOMMON_MYSQL_ENABLED)

SET(LIBRARIES ${LIBRARIES} "${ICU_BASE_LIB_PATH}" "${ICU_DATA_LIB_PATH}")
SET(INCLUDES ${INCLUDES} "${ICU_INCLUDE_PATH}")


SET(SRCS "")


ADD_SUBDIRECTORY(nxcommon)


# The directory containing the generated config.h
SET(INCLUDES ${INCLUDES} "${CMAKE_BINARY_DIR}/build")

# Needed for config.h to reference stdint.h
SET(INCLUDES ${INCLUDES} "${nxcommon_SOURCE_DIR}/nxcommon")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${INCLUDES})

ADD_LIBRARY(nxcommon STATIC ${SRCS})
TARGET_LINK_LIBRARIES(nxcommon ${LIBRARIES})
