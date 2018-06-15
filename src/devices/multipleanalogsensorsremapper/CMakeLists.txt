# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(multipleanalogsensorsremapper
                    CATEGORY device
                    TYPE yarp::dev::MultipleAnalogSensorsRemapper
                    INCLUDE MultipleAnalogSensorsRemapper.h
                    DEFAULT ON)

if(ENABLE_multipleanalogsensorsremapper)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_multipleanalogsensorsremapper MultipleAnalogSensorsRemapper.cpp
                                                     MultipleAnalogSensorsRemapper.h
                                                     $<TARGET_OBJECTS:multipleAnalogSensorsSerializations>)
  target_include_directories(yarp_multipleanalogsensorsclient PRIVATE $<TARGET_PROPERTY:multipleAnalogSensorsSerializations,INTERFACE_INCLUDE_DIRECTORIES>)
  target_link_libraries(yarp_multipleanalogsensorsremapper PRIVATE YARP::YARP_OS
                                                                   YARP::YARP_sig
                                                                   YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_multipleanalogsensorsremapper
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES multipleanalogsensorsremapper.ini
               COMPONENT ${YARP_PLUGIN_MASTER}
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_multipleanalogsensorsremapper PROPERTY FOLDER "Plugins/Device")
endif()
