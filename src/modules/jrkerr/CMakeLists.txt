# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(WIN32)
  if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_plugin(jrkerr
                        CATEGORY device
                        TYPE yarp::dev::JrkerrMotionControl
                        INCLUDE JrkerrMotionControl.h
                        WRAPPER controlboard)
    yarp_install(FILES jrkerr.ini
                 COMPONENT runtime
                 DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  endif()

  if(NOT SKIP_jrkerr)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
    get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_OS_INCLUDE_DIRS}
                        ${YARP_dev_INCLUDE_DIRS})
    yarp_add_plugin(yarp_jrkerr JrkerrMotionControl.cpp
                                nmccom.cpp
                                picservo.cpp
                                sio_util.cpp
                                JrkerrMotionControl.h
                                nmccom.h
                                picservo.h
                                sio_util.h)
    target_link_libraries(yarp_jrkerr YARP_OS
                                      YARP_dev)
    yarp_install(TARGETS yarp_jrkerr
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  endif()
endif()
