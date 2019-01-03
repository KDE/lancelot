
include_directories (
    ${CMAKE_CURRENT_BUILD_DIR}
    )

set (
    BLADE_COMMON_SRCS

    Protocol.cpp

    ui/UiBackend.cpp

    ${VOY_SOURCES}

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
    Qt5::Core
    Qt5::Quick

    ${capnp_LIBRARIES}

    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${ZMQ_LIBRARIES}
    -pthread
    )
