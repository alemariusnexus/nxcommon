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


MACRO(HAS_PARENT_SCOPE OUTVAR)
    GET_DIRECTORY_PROPERTY(pdir PARENT_DIRECTORY)
    
    IF(pdir)
        SET(${OUTVAR} TRUE)
    ELSE()
        SET(${OUTVAR} FALSE)
    ENDIF()
ENDMACRO(HAS_PARENT_SCOPE)


MACRO(PROPAGATE_PARENT_SCOPE)
    HAS_PARENT_SCOPE(haspscope)
    
    IF(haspscope)
        FOREACH(_VAR ${ARGV})
            SET(${_VAR} ${${_VAR}} PARENT_SCOPE)
        ENDFOREACH(_VAR)
    ENDIF()
ENDMACRO(PROPAGATE_PARENT_SCOPE)


MACRO(GLOBAL_ADD_MODULE_PATH BASENAME MODDIR)
    SET(${BASENAME}_CMAKE_MODULE_DIR "${MODDIR}")
    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${BASENAME}_CMAKE_MODULE_DIR}")
    
    PROPAGATE_PARENT_SCOPE(${BASENAME}_CMAKE_MODULE_DIR CMAKE_MODULE_PATH})
ENDMACRO(GLOBAL_ADD_MODULE_PATH)


MACRO(ADD_SOURCES)
    CMAKE_PARSE_ARGUMENTS(ADD_SOURCES "" "OUTVAR" "" ${ARGN})
    
    IF(NOT ADD_SOURCES_OUTVAR)
        SET(ADD_SOURCES_OUTVAR SRCS)
    ENDIF()
    
    FOREACH(_SRC ${ADD_SOURCES_UNPARSED_ARGUMENTS})
        LIST(APPEND ${ADD_SOURCES_OUTVAR} "${CMAKE_CURRENT_SOURCE_DIR}/${_SRC}")
    ENDFOREACH(_SRC)

    PROPAGATE_PARENT_SCOPE(${ADD_SOURCES_OUTVAR})
ENDMACRO(ADD_SOURCES)

#MACRO(ADD_SOURCES)
#    FOREACH(_SRC ${ARGV})
#        SET(SRCS ${SRCS} "${CMAKE_CURRENT_SOURCE_DIR}/${_SRC}")
#    ENDFOREACH(_SRC)
#
#    PROPAGATE_PARENT_SCOPE(SRCS)
#ENDMACRO(ADD_SOURCES)


MACRO(MAKE_VALID_C_IDENTIFIER NAME OUTVAR)
    STRING(REPLACE "-" "_" NAME "${NAME}")
    
    SET(${OUTVAR} "${NAME}")
ENDMACRO(MAKE_VALID_C_IDENTIFIER)


MACRO(TARGET_ADD_INSTALL_STATE_DEFINE TGTNAME TGTID)
    #MAKE_VALID_C_IDENTIFIER("${TGTNAME}" TGTID)
    TARGET_COMPILE_DEFINITIONS(${TGTNAME} PUBLIC $<INSTALL_INTERFACE:__CMAKE_${TGTID}_DUMMY_EXPORT_IS_INSTALLED>)
ENDMACRO(TARGET_ADD_INSTALL_STATE_DEFINE)


#MACRO(EXPORT_FILE_IS_INSTALLED TGTNAME OUTVAR)
#    MAKE_VALID_C_IDENTIFIER("${TGTNAME}" TGTID)
#    
#    GET_PROPERTY(compdefs TARGET "${TGTNAME}" PROPERTY INTERFACE_COMPILE_DEFINITIONS)
#
#    LIST(FIND compdefs "__CMAKE_${TGTID}_DUMMY_EXPORT_IS_INSTALLED" instdefidx1)
#    LIST(FIND compdefs "-D__CMAKE_${TGTID}_DUMMY_EXPORT_IS_INSTALLED" instdefidx2)
#    
#    IF(NOT instdefidx1 EQUAL -1  OR  NOT instdefidx2 EQUAL -1)
#        SET(OUTVAR ON)
#    ELSE()
#        SET(OUTVAR OFF)
#    ENDIF()
#ENDMACRO(EXPORT_FILE_IS_INSTALLED TGTNAME)
