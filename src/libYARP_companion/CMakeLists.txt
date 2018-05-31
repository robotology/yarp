# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_companion)

set(YARP_companion_HDRS include/yarp/companion/yarpcompanion.h)

set(YARP_companion_IMPL_HDRS include/yarp/companion/impl/Companion.h)

set(YARP_companion_SRCS src/Companion.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_companion_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_companion_HDRS}
                   ${YARP_companion_IMPL_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
if(NOT SKIP_ACE)
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})
endif()
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})

add_library(YARP_companion ${YARP_companion_SRCS}
                           ${YARP_companion_HDRS}
                           ${YARP_companion_IMPL_HDRS})
add_library(YARP::YARP_companion ALIAS YARP_companion)

target_include_directories(YARP_companion PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                 $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_companion PUBLIC YARP::YARP_OS)

if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
endif()
if(CMAKE_VERSION VERSION_LESS 3.1)
  if(DEFINED CXX11_FLAGS)
    target_compile_options(YARP_math PUBLIC ${CXX11_FLAGS})
  endif()
else()
  target_compile_features(YARP_math PUBLIC cxx_override)
endif()

if(YARP_HAS_LIBEDIT)
    add_definitions(-DWITH_LIBEDIT)
    include_directories(${Libedit_INCLUDE_DIRS})
    target_link_libraries(YARP_companion PRIVATE ${Libedit_LIBRARIES})
endif()

set_property(TARGET YARP_companion PROPERTY PUBLIC_HEADER ${YARP_companion_HDRS})
set_property(TARGET YARP_companion PROPERTY PRIVATE_HEADER ${YARP_companion_IMPL_HDRS})

install(TARGETS YARP_companion
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/companion"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/companion/impl")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_companion)
set_property(TARGET YARP_companion PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
set_property(TARGET YARP_companion PROPERTY FOLDER "Libraries")
