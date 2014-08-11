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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(nxcommon C CXX)

INCLUDE(TestBigEndian)

ADD_DEFINITIONS(-D__STDC_LIMIT_MACROS -DUCHAR_TYPE=char16_t)

TEST_BIG_ENDIAN(NXCOMMON_BIG_ENDIAN)


SET(NXCOMMON_EXCEPTION_POSITION_INFO OFF CACHE BOOL "If true, exception messages will include the full path to the source file and the line in which the exception was thrown.")
SET(NXCOMMON_SQLITE_ENABLED OFF CACHE BOOL "Enable SQLite support for the nxcommon SQL module. Uses builtin SQLite code.")
SET(NXCOMMON_MYSQL_ENABLED OFF CACHE BOOL "Enable MySQL support for the nxcommon SQL module. Requires libmysqlclient.")
SET(NXCOMMON_PSQL_ENABLED OFF CACHE BOOL "Enable PostgreSQL support for the nxcommon SQL module. Requires libpq.")
SET(NXCOMMON_LUA_ENABLED ON CACHE BOOL "Enable Lua scripting support. Requires liblua.")
SET(NXCOMMON_QT_SUPPORT "off" CACHE STRING "Enables some Qt support, e.g. for converting QStrings to UStrings. (off/qt4/qt5)")
SET(NXCOMMON_BULLET_SUPPORT "off" CACHE STRING "Enables some Bullet support, e.g. for converting Vector3s to btVectors. (off/on)")

IF(NXCOMMON_EXCEPTION_POSITION_INFO)
    MESSAGE(STATUS "Exception position information enabled.")
ELSE(NXCOMMON_EXCEPTION_POSITION_INFO)
    MESSAGE(STATUS "Exception position information disabled.")
ENDIF(NXCOMMON_EXCEPTION_POSITION_INFO)

IF(NXCOMMON_SQLITE_ENABLED)
    MESSAGE(STATUS "SQLite support enabled.")
ELSE(NXCOMMON_SQLITE_ENABLED)
    MESSAGE(STATUS "SQLite support disabled.")
ENDIF(NXCOMMON_SQLITE_ENABLED)

IF(NXCOMMON_MYSQL_ENABLED)
    MESSAGE(STATUS "MySQL support enabled.")
ELSE(NXCOMMON_MYSQL_ENABLED)
    MESSAGE(STATUS "MySQL support disabled.")
ENDIF(NXCOMMON_MYSQL_ENABLED)

IF(NXCOMMON_PSQL_ENABLED)
    MESSAGE(STATUS "PostgreSQL support enabled.")
ELSE(NXCOMMON_PSQL_ENABLED)
    MESSAGE(STATUS "PostgreSQL support disabled.")
ENDIF(NXCOMMON_PSQL_ENABLED)

IF(NXCOMMON_QT_SUPPORT STREQUAL "off")
    MESSAGE(STATUS "Qt support disabled.")
    SET(NXCOMMON_QT_SUPPORT_VERSION 0)
ELSEIF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")
    MESSAGE(STATUS "Qt 4 support enabled.")
    SET(NXCOMMON_QT_SUPPORT_VERSION 4)
ELSEIF(NXCOMMON_QT_SUPPORT STREQUAL "qt5")
    MESSAGE(STATUS "Qt 5 support enabled.")
    SET(NXCOMMON_QT_SUPPORT_VERSION 5)
ELSE(NXCOMMON_QT_SUPPORT STREQUAL "off")
    MESSAGE(WARNING "Invalid value for NXCOMMON_QT_SUPPORT: ${NXCOMMON_QT_SUPPORT}. Possible values: off/qt4/qt5. Qt support will be disabled.")
    SET(NXCOMMON_QT_SUPPORT_VERSION 0)
ENDIF(NXCOMMON_QT_SUPPORT STREQUAL "off")

IF(NXCOMMON_BULLET_SUPPORT STREQUAL "on")
    MESSAGE(STATUS "Bullet support enabled.")
ELSE(NXCOMMON_BULLET_SUPPORT STREQUAL "on")
    MESSAGE(STATUS "Bullet support disabled.")
ENDIF(NXCOMMON_BULLET_SUPPORT STREQUAL "on")


FIND_PATH(ICU_INCLUDE_PATH unicode/utypes.h)
FIND_LIBRARY(ICU_BASE_LIB_PATH NAMES icuuc libicuuc)
FIND_LIBRARY(ICU_DATA_LIB_PATH NAMES icudata libicuudata)

IF(NXCOMMON_MYSQL_ENABLED)
    FIND_LIBRARY(LIBMYSQLCLIENT_LIB_PATH NAMES mysqlclient libmysqlclient mysqlclient18 libmysqlclient18)
    FIND_PATH(LIBMYSQLCLIENT_INCLUDE_DIR mysql/mysql.h)
ENDIF(NXCOMMON_MYSQL_ENABLED)

IF(NXCOMMON_PSQL_ENABLED)
    FIND_LIBRARY(LIBPQ_LIB_PATH NAMES pq libpq)
    FIND_PATH(LIBPQ_INCLUDE_DIR postgresql/libpq-ef.h)
ENDIF(NXCOMMON_PSQL_ENABLED)

IF(NXCOMMON_LUA_ENABLED)
    FIND_PATH(LUA_INCLUDE_PATH lua.h)
    FIND_LIBRARY(LUA_LIB_PATH NAMES lua liblua)
ENDIF(NXCOMMON_LUA_ENABLED)

IF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")
    FIND_PACKAGE(Qt4 4.6.0 COMPONENTS QtCore REQUIRED)
    
    INCLUDE(${QT_USE_FILE})
ELSEIF(NXCOMMON_QT_SUPPORT STREQUAL "qt5")
    FIND_PACKAGE(Qt5Core)
ENDIF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")

IF(NXCOMMON_BULLET_SUPPORT STREQUAL "on")
    FIND_PACKAGE(Bullet COMPONENTS BulletCollision BulletDynamics LinearMath)
ENDIF(NXCOMMON_BULLET_SUPPORT STREQUAL "on")


SET(LIBRARIES "")
SET(INCLUDES "")

IF(NXCOMMON_QT_SUPPORT_ENABLED)
    SET(LIBRARIES ${LIBRARIES} ${QT_LIBRARIES})
ENDIF(NXCOMMON_QT_SUPPORT_ENABLED)

IF(NXCOMMON_MYSQL_ENABLED)
    SET(LIBRARIES ${LIBRARIES} "${LIBMYSQLCLIENT_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${LIBMYSQLCLIENT_INCLUDE_DIR}")
ENDIF(NXCOMMON_MYSQL_ENABLED)

IF(NXCOMMON_PSQL_ENABLED)
    SET(LIBRARIES ${LIBRARIES} "${LIBPQ_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${LIBPQ_INCLUDE_DIR}")
ENDIF(NXCOMMON_PSQL_ENABLED)

IF(NXCOMMON_LUA_ENABLED)
    SET(LIBRARIES ${LIBRARIES} "${LUA_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${LUA_INCLUDE_PATH}")
ENDIF(NXCOMMON_LUA_ENABLED)

IF(NXCOMMON_QT_SUPPORT STREQUAL "qt5")
    SET(LIBRARIES ${LIBRARIES} Qt5::Core)
    SET(INCLUDES ${INCLUDES} ${Qt5Widgets_INCLUDE_DIRS})
ENDIF(NXCOMMON_QT_SUPPORT STREQUAL "qt5")

IF(NXCOMMON_BULLET_SUPPORT STREQUAL "on")
    SET(LIBRARIES ${LIBRARIES} ${BULLET_LIBRARIES})
    SET(INCLUDES ${INCLUDES} ${BULLET_INCLUDE_DIRS})
ENDIF(NXCOMMON_BULLET_SUPPORT STREQUAL "on")

SET(LIBRARIES ${LIBRARIES} "${ICU_BASE_LIB_PATH}" "${ICU_DATA_LIB_PATH}")
SET(INCLUDES ${INCLUDES} "${ICU_INCLUDE_PATH}")


SET(SRCS "")


ADD_SUBDIRECTORY(nxcommon)


# The directory containing the generated config.h
SET(INCLUDES ${INCLUDES} "${CMAKE_BINARY_DIR}/build")

# Needed for config.h to reference stdint.h
SET(INCLUDES ${INCLUDES} "${nxcommon_SOURCE_DIR}/nxcommon")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR} ${INCLUDES})

ADD_LIBRARY(nxcommon STATIC ${SRCS})
TARGET_LINK_LIBRARIES(nxcommon ${LIBRARIES})
