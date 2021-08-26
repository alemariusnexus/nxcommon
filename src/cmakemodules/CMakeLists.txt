# Copyright 2021 David "Alemarius Nexus" Lerch
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

SET(NXCOMMON_CMAKE_MODULE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${NXCOMMON_CMAKE_MODULE_DIR}")

SET(NXCOMMON_CMAKE_MODULE_DIR "${NXCOMMON_CMAKE_MODULE_DIR}" PARENT_SCOPE)
SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
