# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


#### temporary variable for debug
if(YARP_ENABLE_BROKEN_TESTS)
  add_executable(testModule module.cpp)
  target_link_libraries(testModule YARP::YARP_init
                                   YARP::YARP_OS)
  set_property(TARGET testModule PROPERTY FOLDER "Test")
endif()
