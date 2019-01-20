# vim:set softtabstop=3 shiftwidth=3 tabstop=3 expandtab:

project (blade_plugin_baloo)

find_package (KF5Baloo ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package (KF5I18n ${KF5_DEP_VERSION} CONFIG REQUIRED)

include_directories (
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_BINARY_DIR}
   )

set (
   blade_baloo_SRCS
   BalooRunner.cpp
   )

kcoreaddons_add_plugin (
   blade_plugin_baloo
   JSON blade-plugin-baloo.json
   SOURCES ${blade_baloo_SRCS}
   INSTALL_NAMESPACE ${BLADE_PLUGIN_DIR}
   )

target_link_libraries (
   blade_plugin_baloo
   Qt5::Core
   KF5::Baloo
   KF5::CoreAddons
   KF5::I18n
   blade_plugin
   )

set_target_properties (
   blade_plugin_baloo
   PROPERTIES PREFIX ""
   )

