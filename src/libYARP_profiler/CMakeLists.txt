# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(CREATE_LIB_PROFILER)

  project(YARP_profiler)

  set(YARP_profiler_HDRS include/yarp/profiler/NetworkProfiler.h
                         include/yarp/profiler/Graph.h)

  set(YARP_profiler_SRCS src/NetworkProfiler.cpp
                         src/Graph.cpp)

  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Source Files"
               FILES ${YARP_profiler_SRCS})
  source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
               PREFIX "Header Files"
               FILES ${YARP_profiler_HDRS})

  # FIXME All API should use a YARP_profiler_API for __declspec(dllimport/dllexport)
  # For now always build the library as STATIC
  add_library(YARP_profiler STATIC ${YARP_profiler_SRCS}
                                   ${YARP_profiler_HDRS})

  add_library(YARP::YARP_profiler ALIAS YARP_profiler)

  target_include_directories(YARP_profiler PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

  target_link_libraries(YARP_profiler PUBLIC YARP::YARP_OS
                                      PRIVATE YARP::YARP_companion)
  list(APPEND YARP_profiler_PUBLIC_DEPS YARP_OS)
  list(APPEND YARP_profiler_PRIVATE_DEPS YARP_companion)


  if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
  endif()
  if(CMAKE_VERSION VERSION_LESS 3.1)
    if(DEFINED CXX11_FLAGS)
      target_compile_options(YARP_profiler PUBLIC ${CXX11_FLAGS})
    endif()
  else()
    target_compile_features(YARP_profiler PUBLIC cxx_override)
  endif()

  set_property(TARGET YARP_profiler PROPERTY PUBLIC_HEADER ${YARP_profiler_HDRS})
  set_property(TARGET YARP_profiler PROPERTY PRIVATE_HEADER ${YARP_profiler_IMPL_HDRS})
  set_property(TARGET YARP_profiler PROPERTY VERSION ${YARP_VERSION_SHORT})
  set_property(TARGET YARP_profiler PROPERTY SOVERSION ${YARP_SOVERSION})

  install(TARGETS YARP_profiler
          EXPORT YARP_profiler
          COMPONENT YARP_profiler
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/profiler")

  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_profiler
                                   DEPENDENCIES ${YARP_profiler_PUBLIC_DEPS}
                                   PRIVATE_DEPENDENCIES ${YARP_profiler_PRIVATE_DEPS})

  set_property(TARGET YARP_profiler PROPERTY FOLDER "Libraries/Private")
endif()
