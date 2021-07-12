# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# FIXME All API should use a YARP_profiler_API for __declspec(dllimport/dllexport)
# For now always build the library as STATIC
add_library(YARP_profiler STATIC)
add_library(YARP::YARP_profiler ALIAS YARP_profiler)

set(YARP_profiler_HDRS yarp/profiler/NetworkProfiler.h
                       yarp/profiler/Graph.h)

set(YARP_profiler_SRCS yarp/profiler/NetworkProfiler.cpp
                       yarp/profiler/Graph.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_profiler_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_profiler_HDRS})

target_sources(YARP_profiler PRIVATE ${YARP_profiler_SRCS}
                                     ${YARP_profiler_HDRS})

target_include_directories(YARP_profiler PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
                                                $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_profiler PUBLIC cxx_std_14)

target_link_libraries(YARP_profiler PUBLIC YARP::YARP_os
                                    PRIVATE YARP::YARP_companion)
list(APPEND YARP_profiler_PUBLIC_DEPS YARP_os)
list(APPEND YARP_profiler_PRIVATE_DEPS YARP_companion)

set_property(TARGET YARP_profiler PROPERTY PUBLIC_HEADER ${YARP_profiler_HDRS})
set_property(TARGET YARP_profiler PROPERTY PRIVATE_HEADER ${YARP_profiler_IMPL_HDRS})
set_property(TARGET YARP_profiler PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_profiler PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_profiler PROPERTY FOLDER "Libraries/Private")


install(TARGETS YARP_profiler
        EXPORT YARP_profiler
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_profiler
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_profiler
          NAMELINK_COMPONENT YARP_profiler-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_profiler-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/profiler"
          COMPONENT YARP_profiler-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/profiler/impl"
          COMPONENT YARP_profiler-priv-dev)


set(YARP_profiler_PUBLIC_DEPS ${YARP_profiler_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_profiler_PRIVATE_DEPS ${YARP_profiler_PRIVATE_DEPS} PARENT_SCOPE)
