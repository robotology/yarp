# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_serversql)


get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
get_property(YARP_name_INCLUDE_DIRS TARGET YARP_name PROPERTY INCLUDE_DIRS)
include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${YARP_name_INCLUDE_DIRS}
                    "${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories(SYSTEM ${SQLite_INCLUDE_DIRS})

set(YARP_serversql_SRCS src/TripleSourceCreator.cpp
                        src/NameServiceOnTriples.cpp
                        src/AllocatorOnTriples.cpp
                        src/SubscriberOnSql.cpp
                        src/ParseName.cpp
                        src/yarpserver.cpp
                        src/StyleNameService.cpp)

set(YARP_serversql_HDRS include/yarp/serversql/api.h
                        include/yarp/serversql/yarpserversql.h
                        include/yarp/serversql/Server.h)

set(YARP_serversql_IMPL_HDRS include/yarp/serversql/impl/TripleSourceCreator.h
                             include/yarp/serversql/impl/Triple.h
                             include/yarp/serversql/impl/TripleSource.h
                             include/yarp/serversql/impl/SqliteTripleSource.h
                             include/yarp/serversql/impl/NameServiceOnTriples.h
                             include/yarp/serversql/impl/Allocator.h
                             include/yarp/serversql/impl/AllocatorOnTriples.h
                             include/yarp/serversql/impl/Subscriber.h
                             include/yarp/serversql/impl/SubscriberOnSql.h
                             include/yarp/serversql/impl/ComposedNameService.h
                             include/yarp/serversql/impl/ConnectThread.h
                             include/yarp/serversql/impl/ParseName.h
                             include/yarp/serversql/impl/StyleNameService.h)

add_library(YARP_serversql ${YARP_serversql_SRCS}
                           ${YARP_serversql_HDRS}
                           ${YARP_serversql_IMPL_HDRS})

add_library(YARP::YARP_serversql ALIAS YARP_serversql)

target_include_directories(YARP_serversql PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                 $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_serversql PUBLIC YARP::YARP_conf
                                            YARP::YARP_OS
                                            YARP::YARP_init
                                     PRIVATE YARP::YARP_name
                                             ${SQLite_LIBRARIES})
if(UNIX)
  target_link_libraries(YARP_serversql PRIVATE ${CMAKE_DL_LIBS})
endif()


set_property(TARGET YARP_serversql PROPERTY PUBLIC_HEADER ${YARP_serversql_HDRS})
set_property(TARGET YARP_serversql PROPERTY PRIVATE_HEADER ${YARP_serversql_IMPL_HDRS})

install(TARGETS YARP_serversql
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/serversql"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/serversql/impl")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_serversql)
set_property(TARGET YARP_serversql PROPERTY INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
                                                         ${YARP_OS_INCLUDE_DIRS})
set_property(TARGET YARP_serversql PROPERTY FOLDER "Libraries/Private")
