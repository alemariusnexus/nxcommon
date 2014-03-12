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

ADD_DEFINITIONS(-D__STDC_LIMIT_MACROS)

SET(EXCEPTION_POSITION_INFO OFF CACHE BOOL "If true, exception messages will include the full path to the source file and the line in which the exception was thrown.")
SET(NXCOMMON_FILE_IMG_SUPPORT_ENABLED OFF CACHE BOOL "Enable this for the File* classes to support treating IMG archives like plain directories, and IMG entries like plain files (more or less).")

TEST_BIG_ENDIAN(NXCOMMON_BIG_ENDIAN)


#FIND_PATH(ICU_INCLUDE_PATH unicode/utypes.h)
#FIND_LIBRARY(ICU_BASE_LIB_PATH NAMES icuuc libicuuc)


SET(LIBRARIES "")
SET(INCLUDES "")

#SET(LIBRARIES ${LIBRARIES} "${ICU_BASE_LIB_PATH}")
#SET(INCLUDES ${INCLUDES} "${ICU_INCLUDE_PATH}")


SET(SRCS "")


ADD_SUBDIRECTORY(nxcommon)


# The directory containing the generated config.h
SET(INCLUDES ${INCLUDES} "${CMAKE_BINARY_DIR}/build")

# Needed for config.h to reference stdint.h
SET(INCLUDES ${INCLUDES} "${nxcommon_SOURCE_DIR}/nxcommon")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${INCLUDES})

ADD_LIBRARY(nxcommon STATIC ${SRCS})
TARGET_LINK_LIBRARIES(nxcommon ${LIBRARIES})
