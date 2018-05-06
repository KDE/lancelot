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
    )

include (KDEInstallDirs)
include (KDECMakeSettings)


# We can not live without Qt and Boost

set (REQUIRED_QT_VERSION 5.10.0)
find_package (
    Qt5 REQUIRED
    COMPONENTS Gui Qml Quick
    )

find_package (
    Boost 1.67 REQUIRED
    COMPONENTS system filesystem # asio and process are header-only
    )


# Defining the sources

add_subdirectory (src)


