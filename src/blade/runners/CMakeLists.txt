# add_definitions("-DHAVE_SHARED_PTR_STD -DHAVE_CXX0X_UNORDERED")

find_package (KF5Service    ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package (KF5I18n       ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package (KF5Baloo      ${KF5_DEP_VERSION} CONFIG REQUIRED)

set (CMAKE_INCLUDE_CURRENT_DIR ON)

option (BLADE_RECOLL_SUPPORT "Should the recoll runner be built? You also need to set RECOLL_LIB_DIR and RECOLL_SOURCE_DIR" OFF)

# Some vars needed later

set (BLADE_PLUGIN_VERSION 1)
set (BLADE_PLUGIN_DIR "org.kde.plasma.blade/${BLADE_PLUGIN_VERSION}")

configure_file (blade-features.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/blade-features.h)

# Plugin library and plugins

set (
   BladePlugin_SRCS
   AbstractRunner.cpp
   Result.cpp
   )

add_library (
   blade_plugin SHARED
   ${BladePlugin_SRCS}
   )

generate_export_header (blade_plugin)

target_link_libraries (
   blade_plugin
   PUBLIC
   Qt5::Core
   KF5::CoreAddons
   )

add_subdirectory (runners)



# GUI application

set (
   Blade_SRCS

   main.cpp

   Result.cpp
   ResultModel.cpp
   AbstractRunner.cpp

   messages/PingMessage.cpp
   messages/PongMessage.cpp
   messages/QueryMessage.cpp
   messages/ResultMessage.cpp

   agents/Replicant.cpp
   agents/Collector.cpp
   )

qt5_add_resources(
   Blade_SRCS
   ui/main.qrc
   )

add_executable (
   blade
   ${Blade_SRCS}
   )

target_link_libraries (
   blade

   VoyExperimental

   Qt5::Sql
   Qt5::Quick
   Qt5::Qml
   Qt5::Sql
   Qt5::Widgets

   KF5::DNSSD
   KF5::ConfigCore
   KF5::CoreAddons
   KF5::Service
   KF5::I18n

   blade_plugin
   )


# install

install (FILES
   blade.desktop
   DESTINATION ${KDE_INSTALL_KSERVICES5DIR}
   )

install (TARGETS
   blade blade_plugin ${KF5_INSTALL_TARGETS_DEFAULT_ARGS}
   )

install (FILES
   blade_plugin.desktop
   DESTINATION ${KDE_INSTALL_KSERVICETYPES5DIR}
   )
