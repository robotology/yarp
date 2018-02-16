# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_name)

set(YARP_name_HDRS include/yarp/name/all.h
                   include/yarp/name/api.h
                   include/yarp/name/BootstrapServer.h
                   include/yarp/name/NameServerConnectionHandler.h
                   include/yarp/name/NameServerManager.h
                   include/yarp/name/NameService.h)

set(YARP_name_IMPL_HDRS )

set(YARP_name_SRCS src/BootstrapServer.cpp
                   src/NameService.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_name_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_name_HDRS}
                   ${YARP_name_IMPL_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

add_library(YARP_name ${YARP_name_SRCS}
                      ${YARP_name_HDRS}
                      ${YARP_name_IMPL_HDRS})
add_library(YARP::YARP_name ALIAS YARP_name)

target_include_directories(YARP_name PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_name PRIVATE YARP::YARP_OS
                                        ${ACE_LIBRARIES})

if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
endif()
if(CMAKE_VERSION VERSION_LESS 3.1)
  if(DEFINED CXX11_FLAGS)
    target_compile_options(YARP_name PUBLIC ${CXX11_FLAGS})
  endif()
else()
  target_compile_features(YARP_name PUBLIC cxx_override)
endif()

set_property(TARGET YARP_name PROPERTY PUBLIC_HEADER ${YARP_name_HDRS})
set_property(TARGET YARP_name PROPERTY PRIVATE_HEADER ${YARP_name_IMPL_HDRS})

install(TARGETS YARP_name
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/name"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/name/impl")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_name)
set_property(TARGET YARP_name PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
set_property(TARGET YARP_name PROPERTY FOLDER "Libraries/Private")
