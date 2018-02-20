# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_rtf)


set(YARP_rtf_HDRS include/yarp/rtf/JointsPosMotion.h
                  include/yarp/rtf/TestAsserter.h
                  include/yarp/rtf/TestCase.h
                  include/yarp/rtf/api.h)

set(YARP_rtf_SRCS src/JointsPosMotion.cpp
                  src/TestAsserter.cpp
                  src/TestCase.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_rtf_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_rtf_HDRS})

set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

include_directories(${YARP_TREE_INCLUDE_DIRS})
include_directories(SYSTEM ${RTF_INCLUDE_DIRS})

add_library(YARP_rtf ${YARP_rtf_SRCS} ${YARP_rtf_HDRS})
add_library(YARP::YARP_rtf ALIAS YARP_rtf)

target_include_directories(YARP_rtf PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_rtf PUBLIC RTF::RTF
                               PRIVATE YARP::YARP_OS
                                       YARP::YARP_sig
                                       YARP::YARP_init
                                       YARP::YARP_dev)

if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
endif()
if(CMAKE_VERSION VERSION_LESS 3.1)
  if(DEFINED CXX11_FLAGS)
    target_compile_options(YARP_rtf PUBLIC ${CXX11_FLAGS})
  endif()
else()
  target_compile_features(YARP_rtf PUBLIC cxx_nullptr
                                         cxx_override)
endif()

set_property(TARGET YARP_rtf PROPERTY PUBLIC_HEADER ${YARP_rtf_HDRS})

install(TARGETS YARP_rtf
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/yarp/rtf)

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_rtf)
set_property(TARGET YARP_rtf PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
set_property(TARGET YARP_rtf PROPERTY FOLDER "Libraries")
