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

PROJECT(nxcommon-parent C CXX)

SET(NXCOMMON_VERSION_MAJOR "0") # Major version, must be numeric
SET(NXCOMMON_VERSION_MINOR "1") # Minor version, must be numeric
SET(NXCOMMON_VERSION_SUFFIX "-dev") # A suffix for the version string, could e.g. be 'b' for beta versions, or empty

SET(NXCOMMON_CMAKE_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${NXCOMMON_CMAKE_MODULE_DIR}")

INCLUDE(TestBigEndian)
INCLUDE(NxUtilities)
INCLUDE(NxResourceCompiler)

PROPAGATE_PARENT_SCOPE(NXCOMMON_CMAKE_MODULE_DIR CMAKE_MODULE_PATH)


FIND_PACKAGE(Subversion)

SET(_CMAKE_NXCOMMON_HAVE_SVNREV OFF)
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.svn" OR EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.svn")
    IF(Subversion_FOUND)
        Subversion_WC_INFO(${CMAKE_CURRENT_SOURCE_DIR} Project)
        SET(NXCOMMON_VERSION_SVNREV "${Project_WC_REVISION}")
        SET(_CMAKE_NXCOMMON_HAVE_SVNREV ON)
    ELSE(Subversion_FOUND)
        MESSAGE(WARNING "Subversion client not found. The current subversion revision number will not be available to nxcommon!")
    ENDIF(Subversion_FOUND)
ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.svn" OR EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../.svn")

IF(NXCOMMON_VERSION_SVNREV)
    MESSAGE(STATUS "Subversion revision of nxcommon is r${NXCOMMON_VERSION_SVNREV}.")
    
    SET(NXCOMMON_VERSION "${NXCOMMON_VERSION_MAJOR}.${NXCOMMON_VERSION_MINOR}${NXCOMMON_VERSION_SUFFIX}-r${NXCOMMON_VERSION_SVNREV}")
ELSE(NXCOMMON_VERSION_SVNREV)
    SET(NXCOMMON_VERSION "${NXCOMMON_VERSION_MAJOR}.${NXCOMMON_VERSION_MINOR}${NXCOMMON_VERSION_SUFFIX}")
ENDIF(NXCOMMON_VERSION_SVNREV)


SET(NXCOMMON_TEST_BUILD OFF CACHE BOOL "Whether you want to build the unit tests")

CREATE_RESOURCE(libnxcommon/nxcommon/script/scriptsys.lua scriptsys_script)

ADD_SUBDIRECTORY(libnxcommon)

IF(NXCOMMON_TEST_BUILD)
    ADD_SUBDIRECTORY(nxcommon-test)
ENDIF(NXCOMMON_TEST_BUILD)
