cmake_minimum_required (VERSION 3.11)

project (lancelot)

set (PROJECT_VERSION "0.1")
set (PROJECT_VERSION_MAJOR 0)

set (CMAKE_CXX_STANDARD 17)
add_definitions (-fexceptions -pthread)



if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message (FATAL_ERROR "This project requires an out of source build")
endif ()


# There is no using CMake without Extra CMake Modules

include (FeatureSummary)
find_package (ECM 5.46 NO_MODULE)

set_package_properties (
    ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules."
    URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules"
    )
feature_summary (WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)



# Where to search for the custom CMake modules

set (
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
    ${ECM_KDE_MODULE_DIR}
    "${CMAKE_SOURCE_DIR}/cmake/modules"
    )

include (KDEInstallDirs)
include (KDECMakeSettings)


# TODO: Move deps to subfolders where they are actually needed

# Qt and KF5 are needed for the UI

set (REQUIRED_QT_VERSION 5.10.0)
find_package (
    Qt5 REQUIRED
    COMPONENTS Gui Qml Quick
    )
set (CMAKE_AUTOMOC ON)

find_package (
    KF5 REQUIRED
    COMPONENTS Service I18n Baloo Activities Config CoreAddons
    )



# Boost ASIO is needed for the reactive streams event processing

find_package (
    Boost 1.67 REQUIRED
    COMPONENTS system filesystem regex # asio and process are header-only
    )
include_directories (${Boost_INCLUDE_DIR})



# Boost ASIO + ZMQ for the transport

find_package (
    ZMQ REQUIRED
    )
set_package_properties (
    ZMQ PROPERTIES TYPE REQUIRED DESCRIPTION "ZeroMQ is a high-performance asynchronous messaging library"
    URL "http://zeromq.org/"
    )
include_directories (${ZMQ_INCLUDE_DIR} "3rdparty/azmq/")



# Capn Proto is used for message serialization

find_package (
    CapnProto REQUIRED
    )
set_package_properties (
    CapnProto PROPERTIES TYPE REQUIRED DESCRIPTION "Cap'n Proto - efficient data interchange format"
    URL "https://capnproto.org/"
    )
include_directories (${CAPNP_INCLUDE_DIR})

# Sources

add_subdirectory (src)


