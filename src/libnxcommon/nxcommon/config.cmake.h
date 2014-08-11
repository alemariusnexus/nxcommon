/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of nxcommon.

	nxcommon is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	nxcommon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with nxcommon.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef NXCOMMON_CONFIG_H_
#define NXCOMMON_CONFIG_H_

#ifdef _MSC_VER
#	pragma warning(disable : 4996)
#endif

#if defined(__unix) || defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#endif

#include "nxcommon_stdint.h"


#define PI 3.141593f


#if defined(__GNUC__)  &&  defined(__GXX_EXPERIMENTAL_CXX0X__)
#define CXX0X_AVAILABLE
#endif

#cmakedefine NXCOMMON_EXCEPTION_POSITION_INFO
#cmakedefine NXCOMMON_SQLITE_ENABLED
#cmakedefine NXCOMMON_MYSQL_ENABLED
#cmakedefine NXCOMMON_PSQL_ENABLED
#cmakedefine NXCOMMON_LUA_ENABLED
#cmakedefine NXCOMMON_QT_SUPPORT
#cmakedefine NXCOMMON_QT_SUPPORT_VERSION ${NXCOMMON_QT_SUPPORT_VERSION}
#cmakedefine NXCOMMON_BULLET_SUPPORT

#ifdef NXCOMMON_QT_SUPPORT
#define NXCOMMON_QT_SUPPORT_ENABLED
#endif

#ifdef NXCOMMON_BULLET_SUPPORT
#define NXCOMMON_BULLET_SUPPORT_ENABLED
#endif

#cmakedefine _CMAKE_NXCOMMON_HAVE_SVNREV

// Special check on Mac OS X to allow building of universal binaries.
#ifdef __APPLE__
#ifdef __powerpc
#define NXCOMMON_BIG_ENDIAN
#endif
#else
#cmakedefine NXCOMMON_BIG_ENDIAN
#endif

#ifndef NXCOMMON_BIG_ENDIAN
#define NXCOMMON_LITTLE_ENDIAN
#endif

#define NXCOMMON_VERSION_MAJOR ${NXCOMMON_VERSION_MAJOR}
#define NXCOMMON_VERSION_MINOR ${NXCOMMON_VERSION_MINOR}
#define NXCOMMON_VERSION_SUFFIX "${NXCOMMON_VERSION_SUFFIX}"

#ifdef _CMAKE_NXCOMMON_HAVE_SVNREV
#define NXCOMMON_VERSION_SVNREV ${NXCOMMON_VERSION_SVNREV}
#endif

#define NXCOMMON_VERSION "${NXCOMMON_VERSION}"

#endif /* NXCOMMON_CONFIG_H_ */
