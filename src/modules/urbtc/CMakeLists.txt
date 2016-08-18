# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(UNIX)
  if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(urbtc
                        TYPE yarp::dev::UrbtcControl
                        INCLUDE UrbtcControl.h
                        WRAPPER controlboard)
  endif()

  if(NOT SKIP_urbtc)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
    get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_OS_INCLUDE_DIRS}
                        ${YARP_dev_INCLUDE_DIRS})

    yarp_add_plugin(urbtc UrbtcControl.cpp
                          UrbtcControl.h)
    yarp_install(TARGETS urbtc
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  endif()
endif()
