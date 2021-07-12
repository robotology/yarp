# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(RGBDSensorFromRosTopic
                    CATEGORY device
                    TYPE RGBDSensorFromRosTopic
                    INCLUDE RGBDSensorFromRosTopic.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorFromRosTopic)

if(NOT SKIP_RGBDSensorFromRosTopic)
  yarp_add_plugin(yarp_RGBDSensorFromRosTopic)

  target_sources(yarp_RGBDSensorFromRosTopic PRIVATE RGBDSensorFromRosTopic.cpp
                                                     RGBDSensorFromRosTopic.h)
  target_sources(yarp_RGBDSensorFromRosTopic PRIVATE $<TARGET_OBJECTS:RGBDRosConversionUtils>)

  target_include_directories(yarp_RGBDSensorFromRosTopic PRIVATE $<TARGET_PROPERTY:RGBDRosConversionUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_RGBDSensorFromRosTopic PRIVATE YARP::YARP_os
                                                            YARP::YARP_sig
                                                            YARP::YARP_dev
                                                            YARP::YARP_rosmsg)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg
                                                      RGBDRosConversionUtils)

  target_link_libraries(yarp_RGBDSensorFromRosTopic PRIVATE )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS )

  yarp_install(TARGETS yarp_RGBDSensorFromRosTopic
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_RGBDSensorFromRosTopic PROPERTY FOLDER "Plugins/Device")
endif()
