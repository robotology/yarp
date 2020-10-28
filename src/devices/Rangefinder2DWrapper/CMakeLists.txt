# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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
yarp_prepare_plugin(Rangefinder2DWrapperROS
                    CATEGORY device
                    TYPE Rangefinder2DWrapperROS
                    INCLUDE Rangefinder2DWrapperROS.h
                    DEFAULT ON)

if(NOT SKIP_Rangefinder2DWrapper)
  yarp_add_plugin(yarp_Rangefinder2DWrapperROS)

  target_sources(yarp_Rangefinder2DWrapperROS PRIVATE Rangefinder2DWrapperROS.cpp
                                                   Rangefinder2DWrapperROS.h)

  target_link_libraries(yarp_Rangefinder2DWrapperROS PRIVATE YARP::YARP_os
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev
                                                          YARP::YARP_rosmsg)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_Rangefinder2DWrapperROS
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_Rangefinder2DWrapperROS PROPERTY FOLDER "Plugins/Device")
endif()

####################################################################################################
yarp_prepare_plugin(Rangefinder2DWrapperYARP
                    CATEGORY device
                    TYPE Rangefinder2DWrapperYARP
                    INCLUDE Rangefinder2DWrapperYARP.h
                    DEFAULT ON)

if(NOT SKIP_Rangefinder2DWrapper)
  yarp_add_plugin(yarp_Rangefinder2DWrapperYARP)

  target_sources(yarp_Rangefinder2DWrapperYARP PRIVATE Rangefinder2DWrapperYARP.cpp
                                                       Rangefinder2DWrapperYARP.h)

  target_link_libraries(yarp_Rangefinder2DWrapperYARP PRIVATE YARP::YARP_os
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_Rangefinder2DWrapperYARP
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_Rangefinder2DWrapperYARP PROPERTY FOLDER "Plugins/Device")
endif()
