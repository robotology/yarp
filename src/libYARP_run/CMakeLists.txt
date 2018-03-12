# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_run)

set(YARP_run_HDRS include/yarp/run/Run.h)

set(YARP_run_IMPL_HDRS include/yarp/run/impl/RunCheckpoints.h
                       include/yarp/run/impl/RunProcManager.h
                       include/yarp/run/impl/RunReadWrite.h
                       include/yarp/run/impl/PlatformSysPrctl.h)

if(NOT YARP_NO_DEPRECATED) # Since YARP 3.0.0
  set(YARP_run_os_HDRS include/yarp/os/Run.h)
endif()

set(YARP_run_SRCS src/RunCheckpoints.cpp
                  src/Run.cpp
                  src/RunProcManager.cpp
                  src/RunReadWrite.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_run_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_run_HDRS}
                   ${YARP_run_IMPL_HDRS}
                   ${YARP_run_os_HDRS})

include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

add_library(YARP_run ${YARP_run_SRCS}
                     ${YARP_run_HDRS}
                     ${YARP_run_IMPL_HDRS}
                     ${YARP_run_os_HDRS})
add_library(YARP::YARP_run ALIAS YARP_run)

target_include_directories(YARP_run PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(YARP_run PRIVATE YARP::YARP_OS
                                       ${ACE_LIBRARIES})

if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
endif()
if(CMAKE_VERSION VERSION_LESS 3.1)
  if(DEFINED CXX11_FLAGS)
    target_compile_options(YARP_run PUBLIC ${CXX11_FLAGS})
  endif()
else()
  target_compile_features(YARP_run PUBLIC cxx_override)
endif()

set_property(TARGET YARP_run PROPERTY PUBLIC_HEADER ${YARP_run_HDRS})
set_property(TARGET YARP_run PROPERTY PRIVATE_HEADER ${YARP_run_IMPL_HDRS})

install(TARGETS YARP_run
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/run"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/run/impl")
if(NOT YARP_NO_DEPRECATED) # Since YARP 3.0.0
  install(FILES ${YARP_run_os_HDRS}
          COMPONENT deprecated
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/os")
endif()

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_run)
set_property(TARGET YARP_run PROPERTY INCLUDE_DIRS ${YARP_TREE_INCLUDE_DIRS})
set_property(TARGET YARP_run PROPERTY FOLDER "Libraries/Private")
