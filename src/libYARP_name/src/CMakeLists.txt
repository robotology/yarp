# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_name)
add_library(YARP::YARP_name ALIAS YARP_name)

set(YARP_name_HDRS yarp/name/all.h
                   yarp/name/api.h
                   yarp/name/BootstrapServer.h
                   yarp/name/NameServerConnectionHandler.h
                   yarp/name/NameServerManager.h
                   yarp/name/NameService.h)

set(YARP_name_IMPL_HDRS )

set(YARP_name_SRCS yarp/name/BootstrapServer.cpp
                   yarp/name/NameService.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_name_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_name_HDRS}
                   ${YARP_name_IMPL_HDRS})

target_sources(YARP_name PRIVATE ${YARP_name_SRCS}
                                 ${YARP_name_HDRS}
                                 ${YARP_name_IMPL_HDRS})

target_include_directories(YARP_name PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_name PUBLIC cxx_std_14)

target_link_libraries(YARP_name PUBLIC YARP::YARP_conf
                                       YARP::YARP_os)
list(APPEND YARP_name_PUBLIC_DEPS YARP_conf
                                  YARP_os)

if(YARP_HAS_ACE)
  target_compile_definitions(YARP_name PRIVATE YARP_HAS_ACE)
  target_link_libraries(YARP_name PRIVATE ACE::ACE)
  list(APPEND YARP_name_PRIVATE_DEPS ACE)
endif()

set_property(TARGET YARP_name PROPERTY PUBLIC_HEADER ${YARP_name_HDRS})
set_property(TARGET YARP_name PROPERTY PRIVATE_HEADER ${YARP_name_IMPL_HDRS})
set_property(TARGET YARP_name PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_name PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_name PROPERTY FOLDER "Libraries/Private")


install(TARGETS YARP_name
        EXPORT YARP_name
        COMPONENT YARP_name
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_name
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_name
          NAMELINK_COMPONENT YARP_name-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_name-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/name"
          COMPONENT YARP_name-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/name/impl"
          COMPONENT YARP_name-priv-dev)


set(YARP_name_PUBLIC_DEPS ${YARP_os_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_name_PRIVATE_DEPS ${YARP_os_PRIVATE_DEPS} PARENT_SCOPE)
