# vim:set softtabstop=3 shiftwidth=3 tabstop=3 expandtab:

project (blade_plugin_recoll)

include_directories (
   ${CMAKE_CURRENT_SOURCE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/../..
   ${CMAKE_CURRENT_BINARY_DIR}
   )

include_directories (
   ${RECOLL_SOURCE_DIR}/src/rcldb/
   ${RECOLL_SOURCE_DIR}/src/common/
   ${RECOLL_SOURCE_DIR}/src/utils/
   ${RECOLL_SOURCE_DIR}/src/query/
   )

set (
   blade_recoll_SRCS
   RecollRunner.cpp
   )

kcoreaddons_add_plugin (
   blade_plugin_recoll
   JSON blade-plugin-recoll.json
   SOURCES ${blade_recoll_SRCS}
   INSTALL_NAMESPACE ${BLADE_PLUGIN_DIR}
   )

target_link_libraries (
   blade_plugin_recoll
   Qt5::Core
   KF5::CoreAddons
   KF5::I18n
   ${RECOLL_LIB_DIR}/librecoll-1.23.0.so
   blade_plugin
   )

set_target_properties (
   blade_plugin_recoll
   PROPERTIES PREFIX ""
   )

