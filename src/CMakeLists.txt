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

IF(NOT DEFINED NXCOMMON_C_ONLY)
    SET(NXCOMMON_C_ONLY OFF CACHE BOOL "If true, all C++ portions of nxcommon will be disabled.")
ENDIF()


IF(NXCOMMON_C_ONLY)
    PROJECT(nxcommon-parent C)
ELSE()
    PROJECT(nxcommon-parent C CXX)
ENDIF()

IF(NXCOMMON_C_ONLY)
    MESSAGE(STATUS "Building nxcommon in C-only mode")
ELSE()
    MESSAGE(STATUS "Building nxcommon in C++ mode")
ENDIF()

IF(NOT DEFINED NXCOMMON_INSTALL_ENABLED_DEFAULT)
    SET(NXCOMMON_INSTALL_ENABLED_DEFAULT OFF)
ENDIF()
SET(NXCOMMON_INSTALL_ENABLED "${NXCOMMON_INSTALL_ENABLED_DEFAULT}" CACHE BOOL "If true, CMake INSTALL commands will be run for nxcommon.")

SET(NXCOMMON_VERSION_MAJOR "0") # Major version, must be numeric
SET(NXCOMMON_VERSION_MINOR "2") # Minor version, must be numeric
SET(NXCOMMON_VERSION_SUFFIX "-dev") # A suffix for the version string, could e.g. be 'b' for beta versions, or empty

SET(NXCOMMON_CMAKE_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${NXCOMMON_CMAKE_MODULE_DIR}")

INCLUDE(TestBigEndian)
INCLUDE(CheckTypeSize)
INCLUDE(NxUtilities)
INCLUDE(NxResourceCompiler)
INCLUDE(NxLuaDefinitionsParser)
INCLUDE(NxScriptUtilities)
INCLUDE(GetGitRevisionDescription)

PROPAGATE_PARENT_SCOPE(NXCOMMON_CMAKE_MODULE_DIR CMAKE_MODULE_PATH)


# Get Git revision info

# NOTE: The third argument is needed for the ESP-IDF-modified version of GetGitRevisionDescription, where
# it's the repository directory. Doesn't hurt to include it for non-ESP-IDF versions as well
GET_GIT_HEAD_REVISION(NXCOMMON_GIT_REFSPEC NXCOMMON_GIT_SHA1 "${CMAKE_CURRENT_SOURCE_DIR}")

SET(GIT_SHORT_SHA1_LENGTH 7)

IF(DEFINED NXCOMMON_GIT_REFSPEC)
    SET(_CMAKE_NXCOMMON_HAVE_GIT_REFSPEC ON)
ENDIF()
IF(DEFINED NXCOMMON_GIT_SHA1)
    SET(_CMAKE_NXCOMMON_HAVE_GIT_SHA1 ON)
    
    STRING(SUBSTRING "${NXCOMMON_GIT_SHA1}" 0 "${GIT_SHORT_SHA1_LENGTH}" NXCOMMON_GIT_SHA1_SHORT)
ENDIF()

IF(_CMAKE_NXCOMMON_HAVE_GIT_SHA1)
    SET(NXCOMMON_VERSION "${NXCOMMON_VERSION_MAJOR}.${NXCOMMON_VERSION_MINOR}${NXCOMMON_VERSION_SUFFIX}-${NXCOMMON_GIT_SHA1_SHORT}")
ELSE()
    SET(NXCOMMON_VERSION "${NXCOMMON_VERSION_MAJOR}.${NXCOMMON_VERSION_MINOR}${NXCOMMON_VERSION_SUFFIX}")
ENDIF()



IF(NXCOMMON_C_ONLY)
    SET(NXCOMMON_TEST_BUILD OFF)
ELSE()
    SET(NXCOMMON_TEST_BUILD OFF CACHE BOOL "Whether you want to build the unit tests")
ENDIF()

ADD_SUBDIRECTORY(libnxcommon)

IF(NXCOMMON_TEST_BUILD)
    ADD_SUBDIRECTORY(nxcommon-test)
ENDIF(NXCOMMON_TEST_BUILD)
