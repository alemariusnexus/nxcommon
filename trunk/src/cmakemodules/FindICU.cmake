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


FIND_PATH(ICU_INCLUDE_DIR unicode/utypes.h)
FIND_LIBRARY(ICU_BASE_LIBRARY NAMES icuuc libicuuc sicuuc libsicuuc)
FIND_LIBRARY(ICU_DATA_LIBRARY NAMES icudata libicuudata icudt libicudt sicudata libsicudata sicudt libsicudt)

SET(ICU_BASE_LIBRARIES ${ICU_BASE_LIBRARY})
SET(ICU_DATA_LIBRARIES ${ICU_DATA_LIBRARY})
SET(ICU_LIBRARIES ${ICU_BASE_LIBRARIES} ${ICU_DATA_LIBRARIES})

SET(ICU_INCLUDE_DIRS ${ICU_INCLUDE_DIR})

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(ICU DEFAULT_MSG ICU_BASE_LIBRARY ICU_DATA_LIBRARY ICU_INCLUDE_DIR)
