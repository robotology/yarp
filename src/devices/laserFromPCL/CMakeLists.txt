# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(laserFromPCL
                    CATEGORY device
                    TYPE LaserFromPCL
                    INCLUDE laserFromPCL.h)

if(NOT SKIP_laserFromPCL)
  yarp_add_plugin(laserFromPCL)

  target_sources(laserFromPCL PRIVATE laserFromPCL.h
                                      laserFromPCL.cpp)

  target_link_libraries(laserFromPCL PRIVATE YARP::YARP_os
                                               YARP::YARP_sig
                                               YARP::YARP_dev
                                               YARP::YARP_math
                                               YARP::YARP_rosmsg)
                                               
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math
                                                      YARP_rosmsg)

  yarp_install(TARGETS laserFromPCL
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET laserFromPCL PROPERTY FOLDER "Plugins/Device")
endif()
