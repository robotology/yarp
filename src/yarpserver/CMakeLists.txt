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

  set(YARP_serversql_SRCS src/TripleSourceCreator.cpp
                          src/NameServiceOnTriples.cpp
                          src/AllocatorOnTriples.cpp
                          src/SubscriberOnSql.cpp
                          src/ParseName.cpp
                          src/yarpserver3.cpp
                          src/StyleNameService.cpp)
  set(YARP_serversql_HDRS src/TripleSourceCreator.h
                          src/Triple.h
                          src/TripleSource.h
                          src/SqliteTripleSource.h
                          src/NameServiceOnTriples.h
                          src/Allocator.h
                          src/AllocatorOnTriples.h
                          src/Subscriber.h
                          src/SubscriberOnSql.h
                          src/ComposedNameService.h
                          src/ConnectThread.h
                          src/ParseName.h
                          src/StyleNameService.h)
  add_library(YARP_serversql STATIC ${YARP_serversql_SRCS}
                                    ${YARP_serversql_HDRS})
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_OS)
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_init)
  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_name)
  target_link_libraries(YARP_serversql LINK_PRIVATE ${SQLite_LIBRARIES})
  if(UNIX)
    target_link_libraries(YARP_serversql LINK_PRIVATE ${CMAKE_DL_LIBS})
  endif()

  # preserve yarpserver3 executable for a while
  add_executable(yarpserver3 src/main.cpp)
  target_link_libraries(yarpserver3 YARP_serversql)

  add_executable(yarpserver src/main.cpp)
  target_link_libraries(yarpserver YARP_serversql)

  # set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_serversql)

  install(TARGETS YARP_serversql COMPONENT utilities DESTINATION ${CMAKE_INSTALL_LIBDIR})
  install(TARGETS yarpserver yarpserver3 COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

  ###############################################################
  ## Some extra test programs

  if(YARP_COMPILE_TESTS)
    add_executable(server_peek src/server_peek.cpp)
    target_link_libraries(server_peek YARP_serversql)
    add_executable(server_test src/server_test.cpp)
    target_link_libraries(server_test YARP_OS YARP_init)
  endif()

endif()
