# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(serial TYPE yarp::dev::ServerSerial INCLUDE yarp/dev/ServerSerial.h WRAPPER serial)
  yarp_prepare_device(serialport TYPE yarp::dev::SerialDeviceDriver INCLUDE SerialDeviceDriver.h WRAPPER serial)
  yarp_install(FILES serial.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if(SKIP_serial AND SKIP_serialport)
  set(SKIP_all_serial TRUE)
endif()


if(NOT SKIP_all_serial)
  find_package(YARP REQUIRED)
  find_package(ACE)

  if(ACE_FOUND)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                        ${YARP_INCLUDE_DIRS})
    include_directories(SYSTEM ${ACE_INCLUDE_DIRS})
    add_definitions(${YARP_DEFINITIONS})

    yarp_add_plugin(yarp_serial SerialDeviceDriver.cpp SerialDeviceDriver.h)
    target_link_libraries(yarp_serial YARP_OS YARP_dev)
    target_link_libraries(yarp_serial ${ACE_LIBRARIES})
    yarp_install(TARGETS yarp_serial
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  else()
    message(SEND_ERROR "ACE not found; needed by serial driver")
  endif()
endif()
