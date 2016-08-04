# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

get_property(YARP_serversql_INCLUDE_DIRS TARGET YARP_serversql PROPERTY INCLUDE_DIRS)
include_directories(${YARP_serversql_INCLUDE_DIRS})

set(yarpserver_SRCS src/main.cpp)

add_executable(yarpserver ${yarpserver_SRCS})
target_link_libraries(yarpserver YARP_serversql YARP_name)

install(TARGETS yarpserver
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

if(NOT YARP_NO_DEPRECATED) # since YARP 2.3.65, removed in YARP 2.3.68
  add_executable(yarpserver2 src/deprecated.cpp)
  add_executable(yarpserver3 src/deprecated.cpp)
  install(TARGETS yarpserver2
                  yarpserver3
          COMPONENT deprecated
          DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
