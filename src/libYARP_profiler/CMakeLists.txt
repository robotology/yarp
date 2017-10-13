#  Yarp Modules Profiler
#  Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
#  Authors: Nicolo' Genesio <nicolo.genesio@iit.it>
#
#  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
#

if(CREATE_LIB_PROFILER)

  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

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

  set_property(GLOBAL APPEND PROPERTY YARP_TREE_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include")
  get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)

  include_directories(${YARP_TREE_INCLUDE_DIRS})


  # FIXME All API should use a YARP_profiler_API for __declspec(dllimport/dllexport)
  # For now always build the library as STATIC
  add_library(YARP_profiler STATIC ${YARP_profiler_SRCS}
                                  ${YARP_profiler_HDRS})

  add_library(YARP::YARP_profiler ALIAS YARP_profiler)

  target_include_directories(YARP_profiler PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

  target_link_libraries(YARP_profiler PUBLIC YARP::YARP_OS
                                      PRIVATE YARP::YARP_init)


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

  install(TARGETS YARP_profiler
          EXPORT YARP
          COMPONENT runtime
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/profiler")

  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_profiler)
  set_property(TARGET YARP_profiler PROPERTY INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include)
  set_property(TARGET YARP_profiler PROPERTY FOLDER "Libraries/Private")
endif()
