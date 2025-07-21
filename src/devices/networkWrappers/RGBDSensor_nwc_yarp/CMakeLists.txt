# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(RGBDSensor_nwc_yarp
  CATEGORY device
  TYPE RGBDSensor_nwc_yarp
  INCLUDE RGBDSensor_nwc_yarp.h
  DEFAULT ON
  GENERATE_PARSER
)

if(NOT SKIP_RGBDSensorClient)
  yarp_add_plugin(yarp_RGBDSensor_nwc_yarp)

  target_sources(yarp_RGBDSensor_nwc_yarp
    PRIVATE
      RGBDSensor_nwc_yarp.cpp
      RGBDSensor_nwc_yarp.h
      RGBDSensor_nwc_yarp_StreamingMsgParser.cpp
      RGBDSensor_nwc_yarp_StreamingMsgParser.h
      RGBDSensor_nwc_yarp_ParamsParser.cpp
      RGBDSensor_nwc_yarp_ParamsParser.h
  )

  target_sources(yarp_RGBDSensor_nwc_yarp PRIVATE $<TARGET_OBJECTS:RGBDSensorMsgs>)
  target_include_directories(yarp_RGBDSensor_nwc_yarp PRIVATE $<TARGET_PROPERTY:RGBDSensorMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_RGBDSensor_nwc_yarp
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
    TARGETS yarp_RGBDSensor_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_RGBDSensor_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
