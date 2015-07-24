# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (YARP_USE_PERSISTENT_NAMESERVER)

  get_property(YARP_serversql_INCLUDE_DIRS TARGET YARP_serversql PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_serversql_INCLUDE_DIRS})

  # preserve yarpserver3 executable for a while
  add_executable(yarpserver3 src/main.cpp)
  target_link_libraries(yarpserver3 YARP_serversql)

  add_executable(yarpserver src/main.cpp)
  target_link_libraries(yarpserver YARP_serversql)

  install(TARGETS yarpserver yarpserver3 COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})


endif()
