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

CONFIGURE_FILE(config.cmake.h "${CMAKE_CURRENT_BINARY_DIR}/config.h")

ADD_SUBDIRECTORY(config)
ADD_SUBDIRECTORY(exception)
ADD_SUBDIRECTORY(file)
ADD_SUBDIRECTORY(gl)
ADD_SUBDIRECTORY(math)
ADD_SUBDIRECTORY(script)
ADD_SUBDIRECTORY(sql)
ADD_SUBDIRECTORY(stream)

ADD_SOURCES(util strutil CString CRC32 CLIParser Color4 encoding ErrorLog image log ByteArray tinyxml2)

IF(NXCOMMON_LUA_ENABLED)
    ADD_LUASYS_EXPORTS(nxcommon_luadefs util.h)
    CREATE_RESOURCE(nxcommon_res util.lua nxutil_api)
    GENERATE_LUAJIT_CDEF_RESOURCE(nxcommon_res util.h nxutil_ffi)
    
    ADD_LUASYS_EXPORTS(nxcommon_luadefs log.h)
    CREATE_RESOURCE(nxcommon_res log.lua log_api)
    GENERATE_LUAJIT_CDEF_RESOURCE(nxcommon_res log.h log_ffi)
ENDIF()

IF(NXCOMMON_UNICODE_ENABLED)
    ADD_SOURCES(UString unicodeutil)
ENDIF(NXCOMMON_UNICODE_ENABLED)

# Propagate upwards
#ADD_LUASYS_EXPORTS(nxcommon_luadefs)
