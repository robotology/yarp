# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(systemReady_nws_yarp
  CATEGORY device
  TYPE SystemReady_nws_yarp
  INCLUDE SystemReady_nws_yarp.h
  EXTRA_CONFIG
    WRAPPER=systemReady_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_systemReady_nws_yarp)
  yarp_add_plugin(yarp_systemReady_nws_yarp)

  target_sources(yarp_systemReady_nws_yarp
    PRIVATE
      SystemReady_nws_yarp.cpp
      SystemReady_nws_yarp.h
  )

  target_link_libraries(yarp_systemReady_nws_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_systemReady_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_systemReady_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")
endif()

