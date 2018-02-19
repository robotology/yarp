# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

get_property(YARP_serversql_INCLUDE_DIRS TARGET YARP_serversql PROPERTY INCLUDE_DIRS)
include_directories(${YARP_serversql_INCLUDE_DIRS})

set(yarpserver_SRCS src/main.cpp)

add_executable(yarpserver ${yarpserver_SRCS})
target_link_libraries(yarpserver YARP_serversql YARP_name)

install(TARGETS yarpserver
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})
set_property(TARGET yarpserver PROPERTY FOLDER "Command Line Tools")

if(NOT YARP_NO_DEPRECATED) # since YARP 2.3.65, removed in YARP 2.3.68
  add_executable(yarpserver2 src/deprecated.cpp)
  add_executable(yarpserver3 src/deprecated.cpp)
  install(TARGETS yarpserver2
                  yarpserver3
          COMPONENT deprecated
          DESTINATION ${CMAKE_INSTALL_BINDIR})
  set_property(TARGET yarpserver2 yarpserver3 PROPERTY FOLDER "Deprecated")
endif()
