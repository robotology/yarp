# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_serversql)
add_library(YARP::YARP_serversql ALIAS YARP_serversql)

set(YARP_serversql_SRCS
  yarp/serversql/yarpserversql.cpp
  yarp/serversql/Server.cpp
)

set(YARP_serversql_HDRS
  yarp/serversql/api.h
  yarp/serversql/yarpserversql.h
  yarp/serversql/Server.h
)

set(YARP_serversql_IMPL_HDRS
  yarp/serversql/impl/TripleSourceCreator.h
  yarp/serversql/impl/Triple.h
  yarp/serversql/impl/TripleSource.h
  yarp/serversql/impl/SqliteTripleSource.h
  yarp/serversql/impl/NameServiceOnTriples.h
  yarp/serversql/impl/NameServerContainer.h
  yarp/serversql/impl/Allocator.h
  yarp/serversql/impl/AllocatorOnTriples.h
  yarp/serversql/impl/Subscriber.h
  yarp/serversql/impl/SubscriberOnSql.h
  yarp/serversql/impl/ComposedNameService.h
  yarp/serversql/impl/ConnectManager.h
  yarp/serversql/impl/ConnectThread.h
  yarp/serversql/impl/ParseName.h
  yarp/serversql/impl/StyleNameService.h
  yarp/serversql/impl/LogComponent.h
)

set(YARP_serversql_IMPL_SRCS
  yarp/serversql/impl/TripleSourceCreator.cpp
  yarp/serversql/impl/SqliteTripleSource.cpp
  yarp/serversql/impl/ConnectManager.cpp
  yarp/serversql/impl/ConnectThread.cpp
  yarp/serversql/impl/NameServiceOnTriples.cpp
  yarp/serversql/impl/NameServerContainer.cpp
  yarp/serversql/impl/AllocatorOnTriples.cpp
  yarp/serversql/impl/Subscriber.cpp
  yarp/serversql/impl/SubscriberOnSql.cpp
  yarp/serversql/impl/ParseName.cpp
  yarp/serversql/impl/StyleNameService.cpp
  yarp/serversql/impl/LogComponent.cpp
)

source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Source Files"
  FILES
    ${YARP_serversql_SRCS}
    ${YARP_serversql_SRCS}
  )
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Header Files"
  FILES
    ${YARP_serversql_HDRS}
    ${YARP_serversql_IMPL_HDRS}
  )

target_sources(YARP_serversql
  PRIVATE
    ${YARP_serversql_SRCS}
    ${YARP_serversql_HDRS}
    ${YARP_serversql_IMPL_SRCS}
    ${YARP_serversql_IMPL_HDRS}
)

target_include_directories(YARP_serversql
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_serversql PUBLIC cxx_std_14)

target_link_libraries(YARP_serversql
  PUBLIC
    YARP::YARP_conf
    YARP::YARP_os
  PRIVATE
    YARP::YARP_name
    YARP::YARP_init
)
list(APPEND YARP_serversql_PUBLIC_DEPS
  YARP_conf
  YARP_os
)
list(APPEND YARP_serversql_PRIVATE_DEPS YARP_name)

target_include_directories(YARP_serversql SYSTEM PRIVATE ${SQLite_INCLUDE_DIRS})
target_link_libraries(YARP_serversql PRIVATE ${SQLite_LIBRARIES})
list(APPEND YARP_serversql_PRIVATE_DEPS SQLite)


set_property(TARGET YARP_serversql PROPERTY PUBLIC_HEADER ${YARP_serversql_HDRS})
set_property(TARGET YARP_serversql PROPERTY PRIVATE_HEADER ${YARP_serversql_IMPL_HDRS})
set_property(TARGET YARP_serversql PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_serversql PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_serversql PROPERTY FOLDER "Libraries/Private")


install(
  TARGETS YARP_serversql
  EXPORT YARP_serversql
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT YARP_serversql
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_serversql
    NAMELINK_COMPONENT YARP_serversql-dev
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_serversql-dev
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/serversql"
    COMPONENT YARP_serversql-dev
  PRIVATE_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/serversql/impl"
    COMPONENT YARP_serversql-priv-dev)


set(YARP_serversql_PUBLIC_DEPS ${YARP_serversql_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_serversql_PRIVATE_DEPS ${YARP_serversql_PRIVATE_DEPS} PARENT_SCOPE)
