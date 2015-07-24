# Copyright: (C) 2009 RobotCub Consortium
#            (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Paul Fitzpatrick
#          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

  project(YARP_serversql)


  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_name_INCLUDE_DIRS TARGET YARP_name PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_name_INCLUDE_DIRS}
                      "${CMAKE_CURRENT_SOURCE_DIR}/include")


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

  target_link_libraries(YARP_serversql LINK_PRIVATE YARP_OS
                                                    YARP_init
                                                    YARP_name)
  target_link_libraries(YARP_serversql LINK_PRIVATE ${SQLite_LIBRARIES})
  if(UNIX)
    target_link_libraries(YARP_serversql LINK_PRIVATE ${CMAKE_DL_LIBS})
  endif()


  set_property(TARGET YARP_serversql PROPERTY PUBLIC_HEADER ${YARP_serversql_HDRS})
  set_property(TARGET YARP_serversql PROPERTY PRIVATE_HEADER ${YARP_serversql_IMPL_HDRS})

#   install(TARGETS YARP_serversql
#           EXPORT YARP
#           COMPONENT runtime
#           RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
#           LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
#           ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
#           PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/serversql"
#           PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/serversql/impl")

  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_serversql)
  set_property(TARGET YARP_serversql PROPERTY INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
                                                           ${YARP_OS_INCLUDE_DIRS})

    ###############################################################
  ## Some extra test programs

  if(YARP_ENABLE_TESTS)
    add_executable(server_peek src/server_peek.cpp)
    target_link_libraries(server_peek YARP_serversql)

    add_executable(server_test src/server_test.cpp)
    target_link_libraries(server_test YARP_OS YARP_init)
  endif()
