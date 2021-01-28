# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(Rangefinder2DWrapper
                    CATEGORY device
                    TYPE Rangefinder2DWrapper
                    INCLUDE Rangefinder2DWrapper.h
                    DEFAULT ON)

if(NOT SKIP_Rangefinder2DWrapper)
  yarp_add_plugin(yarp_Rangefinder2DWrapper)

  target_sources(yarp_Rangefinder2DWrapper PRIVATE Rangefinder2DWrapper.cpp
                                                   Rangefinder2DWrapper.h)

  target_link_libraries(yarp_Rangefinder2DWrapper PRIVATE YARP::YARP_os
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev
                                                          YARP::YARP_rosmsg)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_Rangefinder2DWrapper
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_Rangefinder2DWrapper PROPERTY FOLDER "Plugins/Device")
endif()

####################################################################################################
yarp_prepare_plugin(rangefinder2D_nws_ros
                    CATEGORY device
                    TYPE Rangefinder2D_nws_ros
                    INCLUDE rangefinder2D_nws_ros.h
                    EXTRA_CONFIG WRAPPER=rangefinder2D_nws_ros
                    DEFAULT ON)

if(NOT SKIP_rangefinder2D_nws_ros)
  yarp_add_plugin(yarp_rangefinder2D_nws_ros)

  target_sources(yarp_rangefinder2D_nws_ros PRIVATE rangefinder2D_nws_ros.cpp
                                                      rangefinder2D_nws_ros.h)

  target_link_libraries(yarp_rangefinder2D_nws_ros PRIVATE YARP::YARP_os
                                                           YARP::YARP_sig
                                                           YARP::YARP_dev
                                                           YARP::YARP_rosmsg)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_rangefinder2D_nws_ros
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_rangefinder2D_nws_ros PROPERTY FOLDER "Plugins/Device")
endif()

####################################################################################################
yarp_prepare_plugin(rangefinder2D_nws_yarp
                    CATEGORY device
                    TYPE Rangefinder2D_nws_yarp
                    INCLUDE rangefinder2D_nws_yarp.h
                    EXTRA_CONFIG WRAPPER=rangefinder2D_nws_yarp
                    DEFAULT ON)

if(NOT SKIP_rangefinder2D_nws_yarp)
  yarp_add_plugin(yarp_rangefinder2D_nws_yarp)

  target_sources(yarp_rangefinder2D_nws_yarp PRIVATE rangefinder2D_nws_yarp.cpp
                                                     rangefinder2D_nws_yarp.h)

  target_link_libraries(yarp_rangefinder2D_nws_yarp PRIVATE YARP::YARP_os
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_rangefinder2D_nws_yarp
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_rangefinder2D_nws_yarp PROPERTY FOLDER "Plugins/Device")
endif()
