# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(laserFromExternalPort
                    CATEGORY device
                    TYPE LaserFromExternalPort
                    INCLUDE laserFromExternalPort.h)

if(NOT SKIP_laserFromExternalPort)
  yarp_add_plugin(laserFromExternalPort)

  target_sources(laserFromExternalPort PRIVATE laserFromExternalPort.h
                                               laserFromExternalPort.cpp)

  target_link_libraries(laserFromExternalPort PRIVATE YARP::YARP_os
                                                      YARP::YARP_sig
                                                      YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS laserFromExternalPort
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET laserFromExternalPort PROPERTY FOLDER "Plugins/Device")
endif()
