# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(CREATE_YARPSERVER3 "Do you want to compile yarpserver3?" FALSE)

if (CREATE_YARPSERVER3)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sqlite)
  include_directories(${ACE_INCLUDE_DIRS})

  set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_name PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

  add_library(yarpserversql STATIC src/TripleSourceCreator.cpp src/TripleSourceCreator.h src/Triple.h src/TripleSource.h src/SqliteTripleSource.h sqlite/sqlite3.c sqlite/sqlite3.h)

  if (NOT WIN32)
    target_link_libraries(yarpserversql pthread dl)
  endif (NOT WIN32)

  add_executable(yarpserver3 ${CMAKE_CURRENT_SOURCE_DIR}/src/yarpserver3.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/NameServiceOnTriples.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/NameServiceOnTriples.h ${CMAKE_CURRENT_SOURCE_DIR}/src/Allocator.h ${CMAKE_CURRENT_SOURCE_DIR}/src/AllocatorOnTriples.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/AllocatorOnTriples.h ${CMAKE_CURRENT_SOURCE_DIR}/src/Subscriber.h ${CMAKE_CURRENT_SOURCE_DIR}/src/SubscriberOnSql.h ${CMAKE_CURRENT_SOURCE_DIR}/src/SubscriberOnSql.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ComposedNameService.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ConnectThread.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ParseName.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ParseName.cpp)
  target_link_libraries(yarpserver3 yarpserversql)
  target_link_libraries(yarpserver3 YARP_init)

  install_targets(/lib yarpserversql)
  install_targets(/bin yarpserver3)

  ###############################################################
  ## Some extra test programs

  IF (TEST_BUILD)
    ADD_EXECUTABLE(server_peek src/server_peek.cpp)
    TARGET_LINK_LIBRARIES(server_peek yarpserversql)
    ADD_EXECUTABLE(server_test src/server_test.cpp)
  ENDIF (TEST_BUILD)

endif (CREATE_YARPSERVER3)
