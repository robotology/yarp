# SPDX-FileCopyrightText: 2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(throttleDown
  TYPE ThrottleDown
  INCLUDE ThrottleDown.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
  DEFAULT ON
)

if(SKIP_ThrottleDown)
  return()
endif()

yarp_add_plugin(yarp_pm_throttleDown)

target_sources(yarp_pm_throttleDown
  PRIVATE
    ThrottleDown.cpp
    ThrottleDown.h
)

target_link_libraries(yarp_pm_throttleDown
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
  YARP_os
  YARP_sig
)

yarp_install(
  TARGETS yarp_pm_throttleDown
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_throttleDown PROPERTY FOLDER "Plugins/Port Monitor")
