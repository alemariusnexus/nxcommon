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


IF(NOT NXCOMMON_C_ONLY)
    ADD_SOURCES(Vector2.cpp Vector3.cpp Vector4.cpp Matrix2.cpp Matrix3.cpp Matrix4.cpp intersection.cpp Quaternion.cpp project.cpp Frustum.cpp mathutil.cpp)
ENDIF()

IF(NXCOMMON_LUA_ENABLED)
    CREATE_RESOURCE(nxcommon_res mathutil.lua mathutil_script)
ENDIF()
