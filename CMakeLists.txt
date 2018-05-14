cmake_minimum_required (VERSION 3.11)

project (lancelot)

set (PROJECT_VERSION "0.1")
set (PROJECT_VERSION_MAJOR 0)

set (CMAKE_CXX_STANDARD 17)
add_definitions (-fexceptions -pthread)



if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message (FATAL_ERROR "This project requires an out of source build")
endif ()



include (FeatureSummary)
find_package (ECM 5.46 NO_MODULE)

set_package_properties (ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary (WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set (
    CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
    ${ECM_KDE_MODULE_DIR}
    "${CMAKE_SOURCE_DIR}/cmake/modules"
    )

include (KDEInstallDirs)
include (KDECMakeSettings)


# Qt for the UI

set (REQUIRED_QT_VERSION 5.10.0)
find_package (
    Qt5 REQUIRED
    COMPONENTS Gui Qml Quick
    )

# Boost ASIO for the reactive streams event processing

find_package (
    Boost 1.67 REQUIRED
    COMPONENTS system filesystem regex # asio and process are header-only
    )
include_directories (${Boost_INCLUDE_DIR})

# Boost ASIO + ZMQ for the transport

find_package (
    ZMQ REQUIRED
    )
include_directories (${ZMQ_INCLUDE_DIR} "3rdparty/azmq/")


# Defining the sources

add_subdirectory (src)


