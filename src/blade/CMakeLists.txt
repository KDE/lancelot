
# Required packages from KF5

find_package (KF5Service    ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package (KF5I18n       ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package (KF5Baloo      ${KF5_DEP_VERSION} CONFIG REQUIRED)


# Some vars needed later

set (BLADE_RUNNER_PLUGIN_VERSION 1)
set (BLADE_RUNNER_PLUGIN_DIR "org.kde.plasma.blade/runners/${BLADE_RUNNER_PLUGIN_VERSION}")

configure_file (blade-features.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/blade-features.h)
include_directories (${CMAKE_CURRENT_BUILD_DIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

set (
    BLADE_COMMON_SRCS

    transport/Serialization.cpp

    ui/UiBackend.cpp

    runners/AbstractRunner.cpp
    runners/plugins/services/ServicesRunner.cpp

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

    transport/protocol/ping_message.capnp
    transport/protocol/query_message.capnp
    transport/protocol/error_message.capnp

    transport/protocol/controller_message.capnp
    )

add_executable (
    bladed
    ${BLADE_COMMON_SRCS}
    ${BLADE_CAPNP_SRCS}
    )

# TODO:
# generate_export_header (blade_plugin)

target_link_libraries (
    bladed
    Qt5::Core
    Qt5::Quick

    KF5::Service
    KF5::I18n

    ${capnp_LIBRARIES}

    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${ZMQ_LIBRARIES}
    -pthread
    )
