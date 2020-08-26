# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

message("BBB Included")

yarp_prepare_plugin(RGBDFromRosTopic
                    CATEGORY device
                    TYPE RGBDFromRosTopic
                    INCLUDE RGBDFromRosTopic.h
                    EXTRA_CONFIG WRAPPER=RGBDSensorWrapper)

if(NOT SKIP_RGBDFromRosTopic)
  yarp_add_plugin(yarp_RGBDFromRosTopic)

  target_sources(yarp_RGBDFromRosTopic PRIVATE RGBDFromRosTopic.cpp
                                         RGBDFromRosTopic.h)
  target_sources(yarp_RGBDFromRosTopic PRIVATE $<TARGET_OBJECTS:RGBDRosConversionUtils>)

  target_include_directories(yarp_RGBDFromRosTopic PRIVATE $<TARGET_PROPERTY:RGBDRosConversionUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_RGBDFromRosTopic PRIVATE YARP::YARP_os
                                                YARP::YARP_sig
                                                YARP::YARP_dev
                                                YARP::YARP_rosmsg)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg
                                                      RGBDRosConversionUtils)

  target_link_libraries(yarp_RGBDFromRosTopic PRIVATE )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS )

  yarp_install(TARGETS yarp_RGBDFromRosTopic
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_RGBDFromRosTopic PROPERTY FOLDER "Plugins/Device")
endif()
