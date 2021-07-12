# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

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

  target_sources(yarp_RGBDSensorWrapper PRIVATE $<TARGET_OBJECTS:framegrabber_protocol>)
  target_include_directories(yarp_RGBDSensorWrapper PRIVATE $<TARGET_PROPERTY:framegrabber_protocol,INTERFACE_INCLUDE_DIRECTORIES>)

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
yarp_prepare_plugin(rgbdSensor_nws_ros
                    CATEGORY device
                    TYPE RgbdSensor_nws_ros
                    INCLUDE RgbdSensor_nws_ros.h
                    EXTRA_CONFIG WRAPPER=rgbdSensor_nws_ros
                    DEFAULT ON)

if(NOT SKIP_rgbdSensor_nws_ros)
  yarp_add_plugin(yarp_rgbdSensor_nws_ros)

  target_sources(yarp_rgbdSensor_nws_ros PRIVATE RgbdSensor_nws_ros.cpp
          RgbdSensor_nws_ros.h)

  target_sources(yarp_rgbdSensor_nws_ros PRIVATE $<TARGET_OBJECTS:framegrabber_protocol>)
  target_include_directories(yarp_rgbdSensor_nws_ros PRIVATE $<TARGET_PROPERTY:framegrabber_protocol,INTERFACE_INCLUDE_DIRECTORIES>)

  target_sources(yarp_rgbdSensor_nws_ros PRIVATE $<TARGET_OBJECTS:RGBDRosConversionUtils>)
  target_include_directories(yarp_rgbdSensor_nws_ros PRIVATE $<TARGET_PROPERTY:RGBDRosConversionUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_rgbdSensor_nws_ros PRIVATE YARP::YARP_os
                                                        YARP::YARP_sig
                                                        YARP::YARP_dev
                                                        YARP::YARP_rosmsg)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_rgbdSensor_nws_ros
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_rgbdSensor_nws_ros PROPERTY FOLDER "Plugins/Device")
endif()
## ROS WRAPPER ###################################################################### END #

## YARP WRAPPER ################################################################### START #
yarp_prepare_plugin(rgbdSensor_nws_yarp
                    CATEGORY device
                    TYPE RgbdSensor_nws_yarp
                    INCLUDE RgbdSensor_nws_yarp.h
                    EXTRA_CONFIG WRAPPER=rgbdSensor_nws_yarp
                    DEFAULT ON)

if(NOT SKIP_rgbdSensor_nws_yarp)
  yarp_add_plugin(yarp_rgbdSensor_nws_yarp)

  target_sources(yarp_rgbdSensor_nws_yarp PRIVATE RgbdSensor_nws_yarp.cpp
          RgbdSensor_nws_yarp.h)

  target_sources(yarp_rgbdSensor_nws_yarp PRIVATE $<TARGET_OBJECTS:framegrabber_protocol>)
  target_include_directories(yarp_rgbdSensor_nws_yarp PRIVATE $<TARGET_PROPERTY:framegrabber_protocol,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_rgbdSensor_nws_yarp PRIVATE YARP::YARP_os
                                                         YARP::YARP_sig
                                                         YARP::YARP_dev)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_rgbdSensor_nws_yarp
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_rgbdSensor_nws_yarp PROPERTY FOLDER "Plugins/Device")
endif()
## YARP WRAPPER ##################################################################### END #
