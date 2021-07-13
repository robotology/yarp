# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(laserFromExternalPort
  CATEGORY device
  TYPE LaserFromExternalPort
  INCLUDE laserFromExternalPort.h
  DEPENDS "TARGET YARP::YARP_math"
)

if(NOT SKIP_laserFromExternalPort)
  yarp_add_plugin(yarp_laserFromExternalPort)

  target_sources(yarp_laserFromExternalPort
    PRIVATE
      laserFromExternalPort.h
      laserFromExternalPort.cpp
  )

  target_link_libraries(yarp_laserFromExternalPort
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
    TARGETS yarp_laserFromExternalPort
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_laserFromExternalPort PROPERTY FOLDER "Plugins/Device")
endif()
