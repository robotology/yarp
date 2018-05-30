# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE

if (YARP_COMPILE_DEVICE_PLUGINS)

  include_directories(${ACE_INCLUDE_DIRS})
  add_executable(yarphear yarphear.cpp)
  target_link_libraries(yarphear YARP::YARP_OS
                                 YARP::YARP_sig
                                 YARP::YARP_dev
                                 YARP::YARP_init)
  target_link_libraries(yarphear ${ACE_LIBRARIES})

  install(TARGETS yarphear
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarphear PROPERTY FOLDER "Command Line Tools")
endif(YARP_COMPILE_DEVICE_PLUGINS)
