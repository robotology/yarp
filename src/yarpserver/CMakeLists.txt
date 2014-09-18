# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (YARP_USE_PERSISTENT_NAMESERVER)

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${SQLite_INCLUDE_DIRS})
  include_directories(${ACE_INCLUDE_DIRS})

  set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)
  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_name PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

  add_library(YARP_serversql STATIC src/TripleSourceCreator.cpp src/TripleSourceCreator.h src/Triple.h src/TripleSource.h src/SqliteTripleSource.h ${CMAKE_CURRENT_SOURCE_DIR}/src/NameServiceOnTriples.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/NameServiceOnTriples.h ${CMAKE_CURRENT_SOURCE_DIR}/src/Allocator.h ${CMAKE_CURRENT_SOURCE_DIR}/src/AllocatorOnTriples.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/AllocatorOnTriples.h ${CMAKE_CURRENT_SOURCE_DIR}/src/Subscriber.h ${CMAKE_CURRENT_SOURCE_DIR}/src/SubscriberOnSql.h ${CMAKE_CURRENT_SOURCE_DIR}/src/SubscriberOnSql.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/ComposedNameService.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ConnectThread.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ParseName.h ${CMAKE_CURRENT_SOURCE_DIR}/src/ParseName.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/yarpserver3.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/StyleNameService.h ${CMAKE_CURRENT_SOURCE_DIR}/src/StyleNameService.cpp)
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_OS)
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_init)
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_name)
  target_link_libraries(YARP_serversql LINK_PRIVATE ${SQLite_LIBRARIES})
  if (UNIX)
    target_link_libraries(YARP_serversql LINK_PRIVATE ${CMAKE_DL_LIBS})
  endif ()

  # preserve yarpserver3 executable for a while
  add_executable(yarpserver3 ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
  target_link_libraries(yarpserver3 YARP_serversql)

  add_executable(yarpserver ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
  target_link_libraries(yarpserver YARP_serversql)

  # set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_serversql)

  install(TARGETS YARP_serversql COMPONENT utilities DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(TARGETS yarpserver yarpserver3 COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  ###############################################################
  ## Some extra test programs

  if(TEST_BUILD)
    add_executable(server_peek src/server_peek.cpp)
    target_link_libraries(server_peek YARP_serversql)
    add_executable(server_test src/server_test.cpp)
  endif()

endif()
