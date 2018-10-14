# Copyright 2010-2015 David "Alemarius Nexus" Lerch
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


IF(NXCOMMON_OPENGL_ENABLED)
    ADD_SOURCES(Camera gl GLException Shader ShaderProgram)
    
    IF(NXCOMMON_LUA_ENABLED)
        ADD_SOURCES(Camera_lua)
        
        ADD_LUASYS_EXPORTS(nxcommon_luadefs Camera_lua.h)
        CREATE_RESOURCE(nxcommon_res camera.lua Camera_api)
        GENERATE_LUAJIT_CDEF_RESOURCE(nxcommon_res Camera_lua.h Camera_ffi)
    ENDIF()
ENDIF(NXCOMMON_OPENGL_ENABLED)
