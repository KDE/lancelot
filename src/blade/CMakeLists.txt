
include_directories (
    ${CMAKE_CURRENT_BUILD_DIR}
    )

set (
    BLADE_COMMON_SRCS

    Protocol.cpp
    main.cpp
    )

if (NOT EXISTS "${CAPNP_EXECUTABLE}")
    # Workaround for some CMake problems with CapnProto
    set(CAPNP_EXECUTABLE "/usr/bin/capnp")
    set(CAPNPC_CXX_EXECUTABLE "/usr/bin/capnpc-c++")
    set(CAPNP_INCLUDE_DIRECTORY "/usr/include")
    set(CAPNP_INCLUDE_DIRS "/usr/include")
endif()

capnp_generate_cpp (
    BLADE_CAPNP_SRCS BLADE_CAPNP_HEADERS

    protocol/ping_message.capnp
    protocol/query_message.capnp
    protocol/error_message.capnp

    protocol/controller_message.capnp
    )

add_executable (
    bladed
    ${BLADE_COMMON_SRCS}
    ${BLADE_CAPNP_SRCS}
    )

target_link_libraries (
    bladed
    ${capnp_LIBRARIES}
    ${ZMQ_LIBRARIES}
    Qt5::Core
    )
