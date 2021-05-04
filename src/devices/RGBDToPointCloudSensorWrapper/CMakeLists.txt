# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.


## ROS WRAPPER #################################################################### START #
yarp_prepare_plugin(RGBDToPointCloudSensor_nws_ros
                    CATEGORY device
                    TYPE RGBDToPointCloudSensor_nws_ros
                    INCLUDE RGBDToPointCloudSensor_nws_ros.h
                    EXTRA_CONFIG WRAPPER=RGBDToPointCloudSensor_nws_ros
                    DEFAULT ON)

if(NOT SKIP_RGBDToPointCloudSensor_nws_ros)
  yarp_add_plugin(RGBDToPointCloudSensor_nws_ros)

  target_sources(RGBDToPointCloudSensor_nws_ros PRIVATE RGBDToPointCloudSensor_nws_ros.cpp
                                                 RGBDToPointCloudSensor_nws_ros.h)
  target_sources(RGBDToPointCloudSensor_nws_ros PRIVATE $<TARGET_OBJECTS:RGBDRosConversionUtils>)

  target_include_directories(RGBDToPointCloudSensor_nws_ros PRIVATE $<TARGET_PROPERTY:RGBDRosConversionUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(RGBDToPointCloudSensor_nws_ros PRIVATE YARP::YARP_os
                                                        YARP::YARP_sig
                                                        YARP::YARP_dev
                                                        YARP::YARP_rosmsg)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS RGBDToPointCloudSensor_nws_ros
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET RGBDToPointCloudSensor_nws_ros PROPERTY FOLDER "Plugins/Device")
endif()
## ROS WRAPPER ###################################################################### END #
