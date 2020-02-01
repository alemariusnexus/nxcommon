# Copyright 2010-2012 David "Alemarius Nexus" Lerch
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


# Resource Compiler
#
# This module provides a simple cross-platform resource compiler. It creates a C header file for each input
# file containing a static array with the file's contents. This way, files can be linked into the executable
# and read using the header. To generate the headers, a simple resource compiler C++ program (rc.cpp) is
# used.
#
# Both the resource compiler itself and the header files are built at build-time, so they can be used with
# resource files that are generated e.g. by a custom command (ADD_CUSTOM_COMMAND()).
#
# The directory containing the generated header files is automatically added to the default include path, and
# resource headers can be included like so:
#
#   #include <res_ALIAS.h>
#
# In cross-compiling mode, a variable BUILD_C_COMPILER must be set to a valid compiler that can be used to
# create programs running on the build machine.
#
#
# CREATE_RESOURCE(TARGET RESFILE ALIAS)
#       Creates a header file calles "res_${ALIAS}.h" containing the contents of the file RESFILE in an
#       unsigned char array of the name "res_${ALIAS}_data". The commands used to build the resource file are
#       added as a dependency to the given TARGET, which is usually a simple custom target created specifically
#       for the resource files.

IF(NOT NX_BUILD_COMPILER_INCLUDED)
    INCLUDE("${CMAKE_CURRENT_LIST_DIR}/NxBuildCompiler.cmake")
ENDIF()


SET(RES_DESTDIR "${CMAKE_CURRENT_BINARY_DIR}/resources")
SET(RES_C_COMPILER "${CMAKE_CURRENT_LIST_DIR}/rc.c")
INCLUDE_DIRECTORIES(${RES_DESTDIR})



MACRO(INITIALIZE_RESOURCE_COMPILER)
    FIND_BUILD_COMPILER()

    IF(NOT TARGET nxcommon_rcc)
        ADD_CUSTOM_COMMAND (
            OUTPUT "${CMAKE_BINARY_DIR}/rcc"
            COMMAND "${BUILD_C_COMPILER}" -o "${CMAKE_BINARY_DIR}/rcc" "${RES_C_COMPILER}"
            MAIN_DEPENDENCY "${RES_C_COMPILER}"
            COMMENT "Compiling resource compiler"
            VERBATIM
            )
        ADD_CUSTOM_TARGET(nxcommon_rcc DEPENDS "${CMAKE_BINARY_DIR}/rcc")
    ENDIF()
ENDMACRO(INITIALIZE_RESOURCE_COMPILER)



MACRO(CREATE_RESOURCE TARGET RESFILE ALIAS)
    SET(DESTFILE "${RES_DESTDIR}/res_${ALIAS}.h")
    
    IF(IS_ABSOLUTE "${RESFILE}")
       SET(SRCFILE "${RESFILE}")
    ELSE(IS_ABSOLUTE "${RESFILE}")
       SET(SRCFILE "${CMAKE_CURRENT_SOURCE_DIR}/${RESFILE}")
    ENDIF(IS_ABSOLUTE "${RESFILE}")
    
    INITIALIZE_RESOURCE_COMPILER()
    
    ADD_CUSTOM_COMMAND (
        OUTPUT "${DESTFILE}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory ${RES_DESTDIR}
        COMMAND "${CMAKE_BINARY_DIR}/rcc" "${SRCFILE}" "${DESTFILE}" "${ALIAS}"
        MAIN_DEPENDENCY "${SRCFILE}"
        DEPENDS nxcommon_rcc
        COMMENT "Compiling resource ${ALIAS}"
        VERBATIM
        )
    
    # This seems to be necessary, because having an ADD_CUSTOM_COMMAND() output as a dependency on
    # an external target does not always work.
    # TODO: Maybe investigate this further.
    ADD_CUSTOM_TARGET(rcc_${ALIAS} ALL DEPENDS "${DESTFILE}")
    
    ADD_DEPENDENCIES("${TARGET}" rcc_${ALIAS})
    
    SET(RESOURCE_FILES ${RESOURCE_FILES} "${DESTFILE}")
ENDMACRO(CREATE_RESOURCE)
