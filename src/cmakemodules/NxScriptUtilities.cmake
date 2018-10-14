IF(NOT NX_BUILD_COMPILER_INCLUDED)
    INCLUDE("${CMAKE_CURRENT_LIST_DIR}/NxBuildCompiler.cmake")
ENDIF()


MACRO(GENERATE_LUAJIT_CDEF_RESOURCE RESTARGET SRCFILE RESALIAS)
    FIND_BUILD_COMPILER()
    
    SET(ABS_SRCFILE "${SRCFILE}")
    
    IF(NOT IS_ABSOLUTE "${ABS_SRCFILE}")
        SET(ABS_SRCFILE "${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE}")
    ENDIF()

    CMAKE_PARSE_ARGUMENTS(GENCDEF "" "" "DEPENDS" ${ARGN})

    SET(PREPROCDIR "${CMAKE_CURRENT_BINARY_DIR}/luajit-ffi-cdefs")
    
    GET_FILENAME_COMPONENT(SRCFILE_BASENAME ${ABS_SRCFILE} NAME)
    SET(PREPROCFILE "${PREPROCDIR}/ffi_${SRCFILE_BASENAME}")

    IF("${BUILD_CXX_COMPILER_ID}" STREQUAL GNU)
        ADD_CUSTOM_COMMAND (
            OUTPUT "${PREPROCFILE}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory ${PREPROCDIR}
            COMMAND "${CMAKE_CXX_COMPILER}" -E -P -DGENERATE_LUAJIT_FFI_CDEF
                -o "${PREPROCFILE}"
                "${ABS_SRCFILE}"
            MAIN_DEPENDENCY "${ABS_SRCFILE}"
            DEPENDS ${GENCDEF_DEPENDS}
            COMMENT "Generating LuaJIT FFI cdefs for resource ${RESALIAS} (${SRCFILE_BASENAME})"
            VERBATIM
        )
    ELSEIF("${BUILD_CXX_COMPILER_ID}" STREQUAL MSVC)
        ADD_CUSTOM_COMMAND (
            OUTPUT "${PREPROCFILE}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory ${PREPROCDIR}
            COMMAND "${CMAKE_CXX_COMPILER}" /EP /DGENERATE_LUAJIT_FFI_CDEF
                "${ABS_SRCFILE}" > "${PREPROCFILE}"
            MAIN_DEPENDENCY "${ABS_SRCFILE}"
            DEPENDS ${GENCDEF_DEPENDS}
            COMMENT "Generating LuaJIT FFI cdefs for resource ${RESALIAS} (${SRCFILE_BASENAME})"
            VERBATIM
        )
    ELSE()
        MESSAGE(SEND_ERROR "Unsupported build compiler for LuaJIT FFI cdef generation: ${BUILD_CXX_COMPILER_ID}")
    ENDIF()
    
    CREATE_RESOURCE(${RESTARGET} "${PREPROCFILE}" "${RESALIAS}")
ENDMACRO(GENERATE_LUAJIT_CDEF_RESOURCE)
