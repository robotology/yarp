# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

option(CREATE_YARPSERVER3 "Do you want to compile yarpserver3?" FALSE)

if (CREATE_YARPSERVER3)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/sqlite)

  GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${ACE_INCLUDE_DIRS})

  ADD_LIBRARY(yarpserversql src/TripleSourceCreator.cpp src/TripleSourceCreator.h src/Triple.h src/TripleSource.h src/SqliteTripleSource.h sqlite/sqlite3.c sqlite/sqlite3.h)

  IF (NOT WIN32)
    TARGET_LINK_LIBRARIES(yarpserversql pthread dl)
  ENDIF (NOT WIN32)

  # Find YARP.  Point the YARP_DIR environment variable at your build.
  # FIND_PACKAGE(YARP)
  ADD_EXECUTABLE(yarpserver3 ${CMAKE_CURRENT_SOURCE_DIR}/src/yarpserver3.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/NameServiceOnTriples.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/NameServiceOnTriples.h ${CMAKE_CURRENT_SOURCE_DIR}/src/NameServerConnectionHandler.h ${CMAKE_CURRENT_SOURCE_DIR}/src/NameServerManager.h ${CMAKE_CURRENT_SOURCE_DIR}/src/BootstrapServer.h ${CMAKE_CURRENT_SOURCE_DIR}/src/BootstrapServer.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/Allocator.h ${CMAKE_CURRENT_SOURCE_DIR}/src/AllocatorOnTriples.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/AllocatorOnTriples.h ${CMAKE_CURRENT_SOURCE_DIR}/src/Subscriber.h ${CMAKE_CURRENT_SOURCE_DIR}/src/SubscriberOnSql.h ${CMAKE_CURRENT_SOURCE_DIR}/src/SubscriberOnSql.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ComposedNameService.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ConnectThread.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ParseName.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ParseName.cpp)
  TARGET_LINK_LIBRARIES(yarpserver3 yarpserversql)
  TARGET_LINK_LIBRARIES(yarpserver3 YARP_init)
  INSTALL_TARGETS(/lib yarpserversql)
  INSTALL_TARGETS(/bin yarpserver3)

  ###############################################################
  ## Some extra test programs

  IF (TEST_BUILD)
    ADD_EXECUTABLE(server_peek src/server_peek.cpp)
    TARGET_LINK_LIBRARIES(server_peek yarpserversql)
    ADD_EXECUTABLE(server_test src/server_test.cpp)
  ENDIF (TEST_BUILD)

endif (CREATE_YARPSERVER3)
