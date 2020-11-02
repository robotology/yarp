# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(RGBDSensorWrapper
                    CATEGORY device
                    TYPE RGBDSensorWrapper
                    INCLUDE RGBDSensorWrapper.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper
                    DEFAULT ON)

if(NOT SKIP_RGBDSensorWrapper)
  yarp_add_plugin(yarp_RGBDSensorWrapper)

  target_sources(yarp_RGBDSensorWrapper PRIVATE RGBDSensorWrapper.cpp
                                                RGBDSensorWrapper.h)
  target_sources(yarp_RGBDSensorWrapper PRIVATE $<TARGET_OBJECTS:RGBDRosConversionUtils>)

  target_include_directories(yarp_RGBDSensorWrapper PRIVATE $<TARGET_PROPERTY:RGBDRosConversionUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_RGBDSensorWrapper PRIVATE YARP::YARP_os
                                                       YARP::YARP_sig
                                                       YARP::YARP_dev
                                                       YARP::YARP_rosmsg)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_RGBDSensorWrapper
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_RGBDSensorWrapper PROPERTY FOLDER "Plugins/Device")
endif()

## ROS WRAPPER #################################################################### START #
yarp_prepare_plugin(RGBDSensorWrapperROS
                    CATEGORY device
                    TYPE RGBDSensorWrapperROS
                    INCLUDE RGBDSensorWrapperROS.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapperROS
                    DEFAULT ON)

if(NOT SKIP_RGBDSensorWrapper)
  yarp_add_plugin(yarp_RGBDSensorWrapperROS)

  target_sources(yarp_RGBDSensorWrapperROS PRIVATE RGBDSensorWrapperROS.cpp
                                                   RGBDSensorWrapperROS.h)
  target_sources(yarp_RGBDSensorWrapperROS PRIVATE $<TARGET_OBJECTS:RGBDRosConversionUtils>)

  target_include_directories(yarp_RGBDSensorWrapperROS PRIVATE $<TARGET_PROPERTY:RGBDRosConversionUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_RGBDSensorWrapperROS PRIVATE YARP::YARP_os
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev
                                                          YARP::YARP_rosmsg)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_RGBDSensorWrapperROS
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_RGBDSensorWrapperROS PROPERTY FOLDER "Plugins/Device")
endif()
## ROS WRAPPER ###################################################################### END #

## YARP WRAPPER ################################################################### START #
yarp_prepare_plugin(RGBDSensorWrapperYARP
                    CATEGORY device
                    TYPE RGBDSensorWrapperYARP
                    INCLUDE RGBDSensorWrapperYARP.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapperYARP
                    DEFAULT ON)

if(NOT SKIP_RGBDSensorWrapper)
  yarp_add_plugin(yarp_RGBDSensorWrapperYARP)

  target_sources(yarp_RGBDSensorWrapperYARP PRIVATE RGBDSensorWrapperYARP.cpp
                                                    RGBDSensorWrapperYARP.h)
  target_sources(yarp_RGBDSensorWrapperYARP PRIVATE $<TARGET_OBJECTS:RGBDRosConversionUtils>)

  target_include_directories(yarp_RGBDSensorWrapperYARP PRIVATE $<TARGET_PROPERTY:RGBDRosConversionUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_RGBDSensorWrapperYARP PRIVATE YARP::YARP_os
                                                           YARP::YARP_sig
                                                           YARP::YARP_dev
                                                           YARP::YARP_rosmsg)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_RGBDSensorWrapperYARP
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_RGBDSensorWrapperYARP PROPERTY FOLDER "Plugins/Device")
endif()
## YARP WRAPPER ##################################################################### END #
