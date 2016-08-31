# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_plugin(serial
                      CATEGORY device
                      TYPE yarp::dev::ServerSerial
                      INCLUDE yarp/dev/ServerSerial.h
                      WRAPPER serial)
  yarp_prepare_plugin(serialport
                      CATEGORY device
                      TYPE yarp::dev::SerialDeviceDriver
                      INCLUDE SerialDeviceDriver.h
                      WRAPPER serial)
  yarp_install(FILES serial.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()


if(NOT SKIP_serial OR NOT SKIP_serialport)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

  yarp_add_plugin(yarp_serial SerialDeviceDriver.cpp
                              SerialDeviceDriver.h)
  target_link_libraries(yarp_serial YARP_OS
                                    YARP_dev
                                    ${ACE_LIBRARIES})
  yarp_install(TARGETS yarp_serial
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()
