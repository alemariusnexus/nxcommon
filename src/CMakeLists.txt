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

PROJECT(nxcommon-parent C CXX)

INCLUDE(TestBigEndian)
INCLUDE(Utilities.cmake)

ADD_DEFINITIONS(-D__STDC_LIMIT_MACROS)

SET(EXCEPTION_POSITION_INFO OFF CACHE BOOL "If true, exception messages will include the full path to the source file and the line in which the exception was thrown.")
SET(NXCOMMON_FILE_IMG_SUPPORT_ENABLED OFF CACHE BOOL "Enable this for the File* classes to support treating IMG archives like plain directories, and IMG entries like plain files (more or less).")

SET(NXCOMMON_TEST_BUILD OFF CACHE BOOL "Whether you want to build the unit tests")

ADD_SUBDIRECTORY(libnxcommon)

IF(NXCOMMON_TEST_BUILD)
    ADD_SUBDIRECTORY(nxcommon-test)
ENDIF(NXCOMMON_TEST_BUILD)
