# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(CREATE_YARPLOGGER)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_logger_INCLUDE_DIRS TARGET YARP_logger PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_logger_INCLUDE_DIRS})
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

  add_executable(yarplogger-console main.cpp)
  target_link_libraries(yarplogger-console YARP::YARP_OS
                                           YARP::YARP_init
                                           YARP::YARP_logger)

  install(TARGETS yarplogger-console COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarplogger-console PROPERTY FOLDER "Command Line Tools")
endif()
