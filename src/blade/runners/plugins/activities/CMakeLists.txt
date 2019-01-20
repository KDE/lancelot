# vim:set softtabstop=3 shiftwidth=3 tabstop=3 expandtab:

project (blade_plugin_activities)

find_package (KF5Activities ${KF5_DEP_VERSION} CONFIG REQUIRED)

include_directories (
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_BINARY_DIR}
   )

set (
   blade_activities_SRCS
   ActivitiesRunner.cpp
   )

kcoreaddons_add_plugin (
   blade_plugin_activities
   JSON blade-plugin-activities.json
   SOURCES ${blade_activities_SRCS}
   INSTALL_NAMESPACE ${BLADE_PLUGIN_DIR}
   )

target_link_libraries (
   blade_plugin_activities
   Qt5::Core
   KF5::Activities
   KF5::CoreAddons
   blade_plugin
   )

set_target_properties (
   blade_plugin_activities
   PROPERTIES PREFIX ""
   )

