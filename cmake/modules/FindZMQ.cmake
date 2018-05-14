# Find ZMQ Headers/Libs

# Variables
# ZMQ_ROOT - set this to a location where ZMQ may be found
#
# ZMQ_FOUND - True of ZMQ found
# ZMQ_INCLUDE_DIR - Location of ZMQ includes
# ZMQ_LIBRARIES - ZMQ libraries

include(FindPackageHandleStandardArgs)

if (NOT ZMQ_ROOT)
    set(ZMQ_ROOT "$ENV{ZMQ_ROOT}")
endif()

if (NOT ZMQ_ROOT)
    find_path(_ZMQ_ROOT NAMES include/zmq.h)
else()
    set(_ZMQ_ROOT "${ZMQ_ROOT}")
endif()

find_path(ZMQ_INCLUDE_DIR NAMES zmq.h HINTS ${_ZMQ_ROOT}/include)

if (ZMQ_INCLUDE_DIR)
    set(_ZMQ_H ${ZMQ_INCLUDE_DIR}/zmq.h)

    function(_zmqver_EXTRACT _ZMQ_VER_COMPONENT _ZMQ_VER_OUTPUT)
        set(CMAKE_MATCH_1 "0")
        set(_ZMQ_expr "^[ \\t]*#define[ \\t]+${_ZMQ_VER_COMPONENT}[ \\t]+([0-9]+)$")
        file(STRINGS "${_ZMQ_H}" _ZMQ_ver REGEX "${_ZMQ_expr}")
        string(REGEX MATCH "${_ZMQ_expr}" ZMQ_ver "${_ZMQ_ver}")
        set(${_ZMQ_VER_OUTPUT} "${CMAKE_MATCH_1}" PARENT_SCOPE)
    endfunction()

    _zmqver_EXTRACT("ZMQ_VERSION_MAJOR" ZMQ_VERSION_MAJOR)
    _zmqver_EXTRACT("ZMQ_VERSION_MINOR" ZMQ_VERSION_MINOR)
    _zmqver_EXTRACT("ZMQ_VERSION_PATCH" ZMQ_VERSION_PATCH)

    message(STATUS "ZMQ version: ${ZMQ_VERSION_MAJOR}.${ZMQ_VERSION_MINOR}.${ZMQ_VERSION_PATCH}")

    # We should provide version to find_package_handle_standard_args in the same format as it was requested,
    # otherwise it can't check whether version matches exactly.
    if (ZMQ_FIND_VERSION_COUNT GREATER 2)
        set(ZMQ_VERSION "${ZMQ_VERSION_MAJOR}.${ZMQ_VERSION_MINOR}.${ZMQ_VERSION_PATCH}")
    else()
        # User has requested ZMQ version without patch part => user is not interested in specific patch =>
        # any patch should be an exact match.
        set(ZMQ_VERSION "${ZMQ_VERSION_MAJOR}.${ZMQ_VERSION_MINOR}")
    endif()

    if (NOT ${CMAKE_CXX_PLATFORM_ID} STREQUAL "Windows")
        find_library(ZMQ_LIBRARIES NAMES zmq HINTS ${_ZMQ_ROOT}/lib)
    else()
        find_library(
            ZMQ_LIBRARY_RELEASE
            NAMES
                libzmq
                "libzmq-${CMAKE_VS_PLATFORM_TOOLSET}-mt-${ZMQ_VERSION_MAJOR}_${ZMQ_VERSION_MINOR}_${ZMQ_VERSION_PATCH}"
            HINTS
                ${_ZMQ_ROOT}/lib
            )

        find_library(
            ZMQ_LIBRARY_DEBUG
            NAMES
                libzmq_d
                "libzmq-${CMAKE_VS_PLATFORM_TOOLSET}-mt-gd-${ZMQ_VERSION_MAJOR}_${ZMQ_VERSION_MINOR}_${ZMQ_VERSION_PATCH}"
            HINTS
                ${_ZMQ_ROOT}/lib)

        # On Windows we have to use corresponding version (i.e. Release or Debug) of ZMQ because of `errno` CRT global variable
        # See more at http://www.drdobbs.com/avoiding-the-visual-c-runtime-library/184416623
        set(ZMQ_LIBRARIES optimized "${ZMQ_LIBRARY_RELEASE}" debug "${ZMQ_LIBRARY_DEBUG}")
    endif()
endif()

find_package_handle_standard_args(ZMQ FOUND_VAR ZMQ_FOUND
    REQUIRED_VARS ZMQ_INCLUDE_DIR ZMQ_LIBRARIES
    VERSION_VAR ZMQ_VERSION)

if (ZMQ_FOUND)
    mark_as_advanced(ZMQ_INCLUDE_DIR ZMQ_LIBRARIES ZMQ_VERSION
        ZMQ_VERSION_MAJOR ZMQ_VERSION_MINOR ZMQ_VERSION_PATCH)
endif()
