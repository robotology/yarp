# Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Alberto Cardellino
# email:   alberto.cardellino@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(fakeIMU
                      TYPE yarp::dev::fakeIMU
                      INCLUDE fakeIMU.h
                      WRAPPER inertial)
  yarp_install(FILES fakeIMU.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if(NOT SKIP_fakeIMU)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
  yarp_add_plugin(yarp_fakeIMU fakeIMU.cpp
                               fakeIMU.h)
  target_link_libraries(yarp_fakeIMU YARP_OS
                                     YARP_sig
                                     YARP_dev)
  yarp_install(TARGETS yarp_fakeIMU
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()

