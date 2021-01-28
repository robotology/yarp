# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_COMPILE_yarprobotinterface)
  add_executable(yarprobotinterface)

  set(yarprobotinterface_SRCS main.cpp
                              Module.cpp)

  set(yarprobotinterface_HDRS Module.h)

  target_sources(yarprobotinterface PRIVATE ${yarprobotinterface_SRCS}
                                            ${yarprobotinterface_HDRS})

  target_link_libraries(yarprobotinterface PRIVATE YARP::YARP_os
                                                   YARP::YARP_dev
                                                   YARP::YARP_init
                                                   YARP::YARP_robotinterface)

  include(YarpChooseIDL)
  yarp_choose_idl(yarprobotinterface_IDLS yarprobotinterface.thrift)
  target_include_directories(yarprobotinterface PRIVATE ${yarprobotinterface_IDLS_BUILD_INTERFACE_INCLUDE_DIRS})
  target_sources(yarprobotinterface PRIVATE ${yarprobotinterface_IDLS_GEN_FILES})

  install(TARGETS yarprobotinterface
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarprobotinterface PROPERTY FOLDER "Command Line Tools")
endif()
