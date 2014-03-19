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

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(nxcommon-test C CXX)

ADD_DEFINITIONS(-DUCHAR_TYPE=char16_t)

ADD_SUBDIRECTORY(src)

IF(nxcommon_SOURCE_DIR)
    SET(NXCOMMON_LIB_PATH nxcommon)
    SET(NXCOMMON_INCLUDE_PATH "${nxcommon_SOURCE_DIR}" "${nxcommon_BINARY_DIR}" "${nxcommon_SOURCE_DIR}/nxcommon")
ELSE(nxcommon_SOURCE_DIR)
    FIND_LIBRARY(NXCOMMON_LIB_PATH NAMES nxcommon libnxcommon)
    FIND_PATH(NXCOMMON_INCLUDE_PATH nxcommon/config.h)
ENDIF(nxcommon_SOURCE_DIR)

FIND_PACKAGE(GTest)

IF(APPLE)
    FIND_PATH(ICONV_INCLUDE_PATH iconv.h)
    FIND_LIBRARY(ICONV_LIB_PATH NAMES iconv libiconv)
ENDIF(APPLE)


SET(LIBRARIES "")
SET(INCLUDES "")

SET(LIBRARIES ${LIBRARIES} "${NXCOMMON_LIB_PATH}")
SET(INCLUDES ${INCLUDES} ${NXCOMMON_INCLUDE_PATH})

SET(LIBRARIES ${LIBRARIES} ${GTEST_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${GTEST_INCLUDE_DIRS})

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_PATH}")
ENDIF(APPLE)

INCLUDE_DIRECTORIES(${INCLUDES})

ADD_EXECUTABLE(nxcommon-test ${SRCS})
TARGET_LINK_LIBRARIES(nxcommon-test ${LIBRARIES})

INSTALL(TARGETS nxcommon-test DESTINATION bin)

