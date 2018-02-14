# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(serialport
                    CATEGORY device
                    TYPE yarp::dev::SerialDeviceDriver
                    INCLUDE SerialDeviceDriver.h
                    EXTRA_CONFIG WRAPPER=serial
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_ACE")

if(NOT SKIP_serialport)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

  yarp_add_plugin(yarp_serialport SerialDeviceDriver.cpp
                                  SerialDeviceDriver.h)
  target_link_libraries(yarp_serialport YARP::YARP_OS
                                        YARP::YARP_dev
                                        ${ACE_LIBRARIES})
  yarp_install(TARGETS yarp_serialport
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES serialport.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_serialport PROPERTY FOLDER "Plugins/Device")
endif()
