# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(rgbdToPointCloudSensor_nws_ros
  CATEGORY device
  TYPE RGBDToPointCloudSensor_nws_ros
  INCLUDE RGBDToPointCloudSensor_nws_ros.h
  EXTRA_CONFIG
    WRAPPER=rgbdToPointCloudSensor_nws_ros
  DEFAULT ON
)

if(NOT SKIP_rgbdToPointCloudSensor_nws_ros)
  yarp_add_plugin(yarp_rgbdToPointCloudSensor_nws_ros)

  target_sources(yarp_rgbdToPointCloudSensor_nws_ros
    PRIVATE
      RGBDToPointCloudSensor_nws_ros.cpp
      RGBDToPointCloudSensor_nws_ros.h
  )

  target_sources(yarp_rgbdToPointCloudSensor_nws_ros PRIVATE $<TARGET_OBJECTS:RGBDRosConversionUtils>)
  target_include_directories(yarp_rgbdToPointCloudSensor_nws_ros PRIVATE $<TARGET_PROPERTY:RGBDRosConversionUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_rgbdToPointCloudSensor_nws_ros
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
    TARGETS yarp_rgbdToPointCloudSensor_nws_ros
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_rgbdToPointCloudSensor_nws_ros PROPERTY FOLDER "Plugins/Device/NWS")
endif()
