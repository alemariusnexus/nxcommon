# Copyright 2010-2014 David "Alemarius Nexus" Lerch
# 
# This file is part of libgta.
#
# gtaformats is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtaformats is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.

IF(NXCOMMON_LUA_ENABLED)
    ADD_SUBDIRECTORY(math)
    ADD_SUBDIRECTORY(sql)

    ADD_SOURCES(luasys.cpp ScriptObject.cpp)
    
    CREATE_RESOURCE(nxcommon_res scriptsys.lua scriptsys_script)
ENDIF(NXCOMMON_LUA_ENABLED)
