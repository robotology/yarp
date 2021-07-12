# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(CMakeRC)

cmrc_add_resource_library(frameTransformRC
                          TYPE OBJECT
                          WHENCE robotinterface_xml
                          PREFIX config_xml
                          robotinterface_xml/ftc_full_ros.xml
                          robotinterface_xml/ftc_pub_ros.xml
                          robotinterface_xml/ftc_ros.xml
                          robotinterface_xml/ftc_sub_ros.xml
                          robotinterface_xml/ftc_pub_yarp_only.xml
                          robotinterface_xml/ftc_sub_yarp_only.xml
                          robotinterface_xml/ftc_yarp_only.xml
                          robotinterface_xml/ftc_yarp_only_single_client.xml
                          robotinterface_xml/ftc_local_only.xml)

yarp_prepare_plugin(frameTransformClient
                    CATEGORY device
                    TYPE FrameTransformClient
                    INCLUDE FrameTransformClient.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_frameTransformClient)
  yarp_add_plugin(yarp_frameTransformClient)

  target_sources(yarp_frameTransformClient PRIVATE FrameTransformClient.cpp
          FrameTransformClient.h)

  target_sources(yarp_frameTransformClient PRIVATE $<TARGET_OBJECTS:FrameTransformUtils>)
  target_include_directories(yarp_frameTransformClient PRIVATE $<TARGET_PROPERTY:FrameTransformUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformClient PRIVATE YARP::YARP_os
                                                          YARP::YARP_sig
                                                          YARP::YARP_dev
                                                          YARP::YARP_math
                                                          YARP::YARP_robotinterface)
  target_sources(yarp_frameTransformClient PRIVATE $<TARGET_OBJECTS:frameTransformRC>)
  target_include_directories(yarp_frameTransformClient PRIVATE $<TARGET_PROPERTY:frameTransformRC,INTERFACE_INCLUDE_DIRECTORIES>)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math
                                                      YARP_robotinterface)

  yarp_install(TARGETS yarp_frameTransformClient
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformClient PROPERTY FOLDER "Plugins/Device")
endif()
