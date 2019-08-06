# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_serversql)

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
                                            YARP::YARP_os
                                     PRIVATE YARP::YARP_name
                                             YARP::YARP_init)
list(APPEND YARP_serversql_PUBLIC_DEPS YARP_conf
                                       YARP_os)
list(APPEND YARP_serversql_PRIVATE_DEPS YARP_name)

target_include_directories(YARP_serversql SYSTEM PRIVATE ${SQLite_INCLUDE_DIRS})
target_link_libraries(YARP_serversql PRIVATE ${SQLite_LIBRARIES})
list(APPEND YARP_serversql_PRIVATE_DEPS SQLite)


set_property(TARGET YARP_serversql PROPERTY PUBLIC_HEADER ${YARP_serversql_HDRS})
set_property(TARGET YARP_serversql PROPERTY PRIVATE_HEADER ${YARP_serversql_IMPL_HDRS})
set_property(TARGET YARP_serversql PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_serversql PROPERTY SOVERSION ${YARP_SOVERSION})

install(TARGETS YARP_serversql
        EXPORT YARP_serversql
        COMPONENT YARP_serversql
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/serversql"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/serversql/impl")

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_serversql
                                 DEPENDENCIES ${YARP_serversql_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_serversql_PRIVATE_DEPS})

set_property(TARGET YARP_serversql PROPERTY FOLDER "Libraries/Private")
