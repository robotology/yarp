# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(analogServer
  CATEGORY device
  TYPE AnalogWrapper
  INCLUDE AnalogWrapper.h
  EXTRA_CONFIG
    WRAPPER=analogServer
  DEFAULT ON
)

if(NOT SKIP_analogServer)
  yarp_add_plugin(yarp_analogServer)

  target_sources(yarp_analogServer
    PRIVATE
      AnalogWrapper.cpp
      AnalogWrapper.h
  )

  target_link_libraries(yarp_analogServer
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_rosmsg
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_rosmsg
  )

  yarp_install(
    TARGETS yarp_analogServer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_analogServer PROPERTY FOLDER "Plugins/Device")
endif()
