# Copyright 2010-2014 David "Alemarius Nexus" Lerch
# 
# This file is part of gtatools.
#
# gtatools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtatools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtatools.  If not, see <http://www.gnu.org/licenses/>.
#
# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.

IF(nxcommon_SOURCE_DIR)
    MESSAGE(STATUS "Building libnxcommon in ${nxcommon_SOURCE_DIR}")
    SET(LIBNXCOMMON_LIBRARY nxcommon CACHE FILEPATH "libnxcommon library file" FORCE)
    SET(LIBNXCOMMON_INCLUDE_DIR "${nxcommon_SOURCE_DIR}" "${nxcommon_BINARY_DIR}" "${nxcommon_SOURCE_DIR}/nxcommon" CACHE PATH "libnxcommon include directories" FORCE)
ELSE(nxcommon_SOURCE_DIR)
    MESSAGE(STATUS "Using external libnxcommon")
    FIND_LIBRARY(LIBNXCOMMON_LIBRARY NAMES nxcommon libnxcommon)
    FIND_PATH(LIBNXCOMMON_INCLUDE_DIR nxcommon/config.h)
ENDIF(nxcommon_SOURCE_DIR)

SET(LIBNXCOMMON_LIBRARIES ${LIBNXCOMMON_LIBRARY})
SET(LIBNXCOMMON_INCLUDE_DIRS ${LIBNXCOMMON_INCLUDE_DIR})

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libnxcommon LIBNXCOMMON_LIBRARY LIBNXCOMMON_INCLUDE_DIR)
