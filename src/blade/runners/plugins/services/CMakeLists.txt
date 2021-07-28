# vim:set softtabstop=3 shiftwidth=3 tabstop=3 expandtab:

project (blade_plugin_services)

find_package (KF5Service    ${KF5_DEP_VERSION} CONFIG REQUIRED)
find_package (KF5I18n       ${KF5_DEP_VERSION} CONFIG REQUIRED)

include_directories (
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_BINARY_DIR}
   )

set (
   blade_services_SRCS
   ServicesRunner.cpp
   )

kcoreaddons_add_plugin (
   blade_plugin_services
   SOURCES ${blade_services_SRCS}
   INSTALL_NAMESPACE ${BLADE_PLUGIN_DIR}
   )

target_link_libraries (
   blade_plugin_services

   Qt5::Core

   KF5::Service
   KF5::I18n
   KF5::CoreAddons

   blade_plugin
   )

set_target_properties (
   blade_plugin_services
   PROPERTIES PREFIX ""
   )

