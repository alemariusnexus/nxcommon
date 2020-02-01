

MACRO(FIND_BUILD_COMPILER)
    IF(CMAKE_CROSSCOMPILING)
        # TODO: Add auto-detection
        SET(BUILD_C_COMPILER "" CACHE FILEPATH "Path to C compiler that can be used to build programs running on the build machine.")
        SET(BUILD_C_COMPILER_ID "" CACHE STRING "Type of the C build compiler in BUILD_C_COMPILER (e.g. GNU, MSVC)")
    ELSE()
        SET(BUILD_C_COMPILER "${CMAKE_C_COMPILER}")
        SET(BUILD_C_COMPILER_ID "${CMAKE_C_COMPILER_ID}")
    ENDIF()
    
    IF(NOT EXISTS "${BUILD_C_COMPILER}")
        MESSAGE(SEND_ERROR "Variable BUILD_C_COMPILER does not point to a valid build compiler!")
    ENDIF()
    IF("z${BUILD_C_COMPILER_ID}" STREQUAL "z")
        MESSAGE(SEND_ERROR "Variable BUILD_C_COMPILER_ID is not set!")
    ENDIF()
    
    #IF(NOT DEFINED BUILD_CXX_COMPILER_ID)
    #    # Find compiler type
    #    EXECUTE_PROCESS(COMMAND "${BUILD_CXX_COMPILER}" "--version" TIMEOUT 5 OUTPUT_VARIABLE _compout ERROR_VARIABLE _comperr)
    #    
    #    STRING(FIND "${_compout}" "Microsoft (R)" _mspos)
    #    IF(${_mspos} GREATER_EQUAL 0)
    #        SET(_guessed_id "MSVC")
    #    ENDIF()
    #    STRING(FIND "${_comperr}" "Microsoft (R)" _mspos)
    #    IF(${_mspos} GREATER_EQUAL 0)
    #        SET(_guessed_id "MSVC")
    #    ENDIF()
    #    
    #    IF(NOT DEFINED _guessed_id)
    #        SET(_guessed_id "GNU")
    #    ENDIF()
    #    
    #    SET(BUILD_CXX_COMPILER_ID "${_guessed_id}" CACHE STRING "Type of the C++ build compiler (e.g. GNU, MSVC)")
    #ENDIF()
ENDMACRO(FIND_BUILD_COMPILER)


SET(NX_BUILD_COMPILER_INCLUDED TRUE)
