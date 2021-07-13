# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(analogsensorclient
  CATEGORY device
  TYPE AnalogSensorClient
  INCLUDE AnalogSensorClient.h
  DEFAULT ON
)

if(NOT SKIP_analogsensorclient)
  yarp_add_plugin(yarp_analogsensorclient)

  target_sources(yarp_analogsensorclient
    PRIVATE
      AnalogSensorClient.cpp
      AnalogSensorClient.h
  )

  target_link_libraries(yarp_analogsensorclient
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
    TARGETS yarp_analogsensorclient
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_analogsensorclient PROPERTY FOLDER "Plugins/Device")
endif()
