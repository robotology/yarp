# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_yarplogger)
  add_executable(yarplogger-console)
  target_sources(yarplogger-console PRIVATE main.cpp)
  target_link_libraries(yarplogger-console PRIVATE YARP::YARP_os
                                                   YARP::YARP_init
                                                   YARP::YARP_logger)

  install(TARGETS yarplogger-console COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarplogger-console PROPERTY FOLDER "Command Line Tools")
endif()
