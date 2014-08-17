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

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(nxcommon-test C CXX)


ADD_SUBDIRECTORY(src)

IF(NOT TARGET nxcommon)
    FIND_PACKAGE(Nxcommon REQUIRED CONFIG)
ENDIF()

INCLUDE(NxUtilities)

FIND_PACKAGE(GTest REQUIRED)
FIND_PACKAGE(ICU REQUIRED)

IF(APPLE)
    FIND_PATH(ICONV_INCLUDE_PATH iconv.h)
    FIND_LIBRARY(ICONV_LIB_PATH NAMES iconv libiconv)
ENDIF(APPLE)


SET(LIBRARIES "")
SET(INCLUDES "")

#SET(LIBRARIES ${LIBRARIES} "${LIBNXCOMMON_LIBRARIES}")
#SET(INCLUDES ${INCLUDES} ${LIBNXCOMMON_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} ${GTEST_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${GTEST_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} ${ICU_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${ICU_INCLUDE_DIRS})

IF(UNIX)
    SET(CMAKE_THREAD_PREFER_PTHREAD ON)
    FIND_PACKAGE(Threads)
    
    SET(LIBRARIES ${LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
ENDIF(UNIX)

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_PATH}")
ENDIF(APPLE)

# For the generated config.h
INCLUDE_DIRECTORIES(${INCLUDES} "${nxcommon-test_BINARY_DIR}/includes")

ADD_EXECUTABLE(nxcommon-test ${SRCS})
TARGET_LINK_LIBRARIES(nxcommon-test ${LIBRARIES} nxcommon)

INSTALL(TARGETS nxcommon-test DESTINATION bin)

