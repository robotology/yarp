# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(multipleanalogsensorsserver
                    CATEGORY device
                    TYPE MultipleAnalogSensorsServer
                    INCLUDE MultipleAnalogSensorsServer.h
                    EXTRA_CONFIG WRAPPER=multipleanalogsensorsserver
                    DEFAULT ON)

if(ENABLE_multipleanalogsensorsserver)
  yarp_add_plugin(yarp_multipleanalogsensorsserver)

  target_sources(yarp_multipleanalogsensorsserver PRIVATE MultipleAnalogSensorsServer.cpp
                                                          MultipleAnalogSensorsServer.h)

  target_sources(yarp_multipleanalogsensorsserver PRIVATE $<TARGET_OBJECTS:multipleAnalogSensorsSerializations>)
  target_include_directories(yarp_multipleanalogsensorsserver PRIVATE $<TARGET_PROPERTY:multipleAnalogSensorsSerializations,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_multipleanalogsensorsserver PRIVATE YARP::YARP_os
                                                                 YARP::YARP_sig
                                                                 YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_multipleanalogsensorsserver
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_multipleanalogsensorsserver PROPERTY FOLDER "Plugins/Device")
endif()
