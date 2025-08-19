# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(simulated_network_delay
  TYPE Simulated_network_delay
  INCLUDE Simulated_network_delay.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
  DEFAULT ON
)

if(SKIP_simulated_network_delay)
  return()
endif()

yarp_add_plugin(yarp_pm_simulated_network_delay)

target_sources(yarp_pm_simulated_network_delay
  PRIVATE
    Simulated_network_delay.cpp
    Simulated_network_delay.h
)

target_link_libraries(yarp_pm_simulated_network_delay
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
)

yarp_install(
  TARGETS yarp_pm_simulated_network_delay
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_simulated_network_delay PROPERTY FOLDER "Plugins/Port Monitor")
