# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(serialport
                    CATEGORY device
                    TYPE yarp::dev::SerialDeviceDriver
                    INCLUDE SerialDeviceDriver.h
                    EXTRA_CONFIG WRAPPER=serial
                    DEPENDS "YARP_HAS_ACE")

if(NOT SKIP_serialport)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_serialport SerialDeviceDriver.cpp
                                  SerialDeviceDriver.h)
  target_link_libraries(yarp_serialport PRIVATE YARP::YARP_OS
                                                YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_dev)

  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})
  target_link_libraries(yarp_serialport PRIVATE ${ACE_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ACE) (not using targets)

  yarp_install(TARGETS yarp_serialport
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES serialport.ini
               COMPONENT ${YARP_PLUGIN_MASTER}
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_serialport PROPERTY FOLDER "Plugins/Device")
endif()
