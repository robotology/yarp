# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
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

  include(YarpIDL)
  yarp_idl_to_dir(INPUT_FILES yarprobotinterface.thrift
                  OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code"
                  SOURCES_VAR yarprobotinterface_IDLS_SRCS
                  HEADERS_VAR yarprobotinterface_IDLS_HDRS
                  INCLUDE_DIRS_VAR yarprobotinterface_IDLS_INCLUDE_DIRS)
  target_include_directories(yarprobotinterface PRIVATE ${yarprobotinterface_IDLS_INCLUDE_DIRS})
  target_sources(yarprobotinterface PRIVATE ${yarprobotinterface_IDLS_SRCS}
                                            ${yarprobotinterface_IDLS_HDRS})

  install(TARGETS yarprobotinterface
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarprobotinterface PROPERTY FOLDER "Command Line Tools")
endif()
