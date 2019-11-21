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

SET(LUA_DEF_PARSER "${CMAKE_CURRENT_LIST_DIR}/luadefparser.cpp")
SET(LUA_DEF_GENFILE "${CMAKE_CURRENT_BINARY_DIR}/luadefs.cpp")


MACRO(ADD_LUASYS_EXPORTS TARGET) #SRCFILE)
    FOREACH(_SRC ${ARGN})
        IF(IS_ABSOLUTE "${_SRC}")
           SET(_ABS_SRC "${_SRC}")
        ELSE()
           SET(_ABS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${_SRC}")
        ENDIF()
        
        LIST(APPEND "LUA_DEF_FILES_${TARGET}" "${_ABS_SRC}")
    ENDFOREACH(_SRC)
    
    PROPAGATE_PARENT_SCOPE("LUA_DEF_FILES_${TARGET}")
ENDMACRO(ADD_LUASYS_EXPORTS)


MACRO(RUN_LUASYS_EXPORTS_PARSER TARGET OUTSRCFILE)
    IF(NOT TARGET "${TARGET}")
        ADD_CUSTOM_TARGET("${TARGET}")
    ENDIF(NOT TARGET "${TARGET}")

    FIND_BUILD_COMPILER()

    IF(NOT TARGET nxcommon_ldp)
        ADD_CUSTOM_COMMAND (
            OUTPUT "${CMAKE_BINARY_DIR}/luadefparser"
            COMMAND "${BUILD_CXX_COMPILER}" -o "${CMAKE_BINARY_DIR}/luadefparser" "${LUA_DEF_PARSER}"
            MAIN_DEPENDENCY "${LUA_DEF_PARSER}"
            COMMENT "Compiling Luasys exports parser"
            VERBATIM
            )
        ADD_CUSTOM_TARGET(nxcommon_ldp DEPENDS "${CMAKE_BINARY_DIR}/luadefparser")
    ENDIF()
    
    ADD_CUSTOM_COMMAND (
        OUTPUT "${LUA_DEF_GENFILE}"
        COMMAND "${CMAKE_BINARY_DIR}/luadefparser" "${LUA_DEF_GENFILE}" ${LUA_DEF_FILES_${TARGET}}
        DEPENDS nxcommon_ldp ${LUA_DEF_FILES_${TARGET}}
        COMMENT "Parsing Luasys exports"
        VERBATIM
        )
    
    # This seems to be necessary, because having an ADD_CUSTOM_COMMAND() output as a dependency on
    # an external target does not always work.
    # TODO: Maybe investigate this further.
    ADD_CUSTOM_TARGET("nxcommon_ldp_${TARGET}" ALL DEPENDS "${LUA_DEF_GENFILE}")
    
    ADD_DEPENDENCIES("${TARGET}" "nxcommon_ldp_${TARGET}")
    
    SET("${OUTSRCFILE}" "${LUA_DEF_GENFILE}")
ENDMACRO(RUN_LUASYS_EXPORTS_PARSER)




MACRO(ADD_LUA_DEFINITION_FILE_OLD TARGET SRCFILE)
    SET(DESTFILE "${RES_DESTDIR}/res_${ALIAS}.h")
    
    IF(IS_ABSOLUTE "${SRCFILE}")
       SET(ABS_SRCFILE "${RESFILE}")
    ELSE(IS_ABSOLUTE "${SRCFILE}")
       SET(ABS_SRCFILE "${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}")
    ENDIF(IS_ABSOLUTE "${SRCFILE}")
    
    INITIALIZE_LUA_DEFINITIONS_PARSER()
    
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
ENDMACRO(ADD_LUA_DEFINITION_FILE_OLD)
