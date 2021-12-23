# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(rpc_monitor
  TYPE RpcMonitor
  INCLUDE RpcMonitor.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
  DEFAULT ON
)

if(SKIP_rpc_monitor)
  return()
endif()

yarp_add_plugin(yarp_pm_rpc_monitor)

target_sources(yarp_pm_rpc_monitor
  PRIVATE
    RpcMonitor.cpp
    RpcMonitor.h
)
target_link_libraries(yarp_pm_rpc_monitor
  PRIVATE
    YARP::YARP_os
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
  YARP_os
)

yarp_install(
  TARGETS yarp_pm_rpc_monitor
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_rpc_monitor PROPERTY FOLDER "Plugins/Port Monitor")
