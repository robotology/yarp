# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(RGBDSensorClient
                    CATEGORY device
                    TYPE RGBDSensorClient
                    INCLUDE RGBDSensorClient.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorClient
                    DEFAULT ON)

if(NOT SKIP_RGBDSensorClient)
  yarp_add_plugin(yarp_RGBDSensorClient)

  target_sources(yarp_RGBDSensorClient PRIVATE RGBDSensorClient.cpp
                                               RGBDSensorClient.h
                                               RGBDSensorClient_StreamingMsgParser.cpp
                                               RGBDSensorClient_StreamingMsgParser.h)

  target_sources(yarp_RGBDSensorClient PRIVATE $<TARGET_OBJECTS:framegrabber_protocol>)
  target_include_directories(yarp_RGBDSensorClient PRIVATE $<TARGET_PROPERTY:framegrabber_protocol,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_RGBDSensorClient PRIVATE YARP::YARP_os
                                                      YARP::YARP_sig
                                                      YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_RGBDSensorClient
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_RGBDSensorClient PROPERTY FOLDER "Plugins/Device")
endif()
