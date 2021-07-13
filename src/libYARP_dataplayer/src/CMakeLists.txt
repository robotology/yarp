# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# FIXME All API should use a YARP_dataplayer_API for __declspec(dllimport/dllexport)
# For now always build the library as STATIC
add_library(YARP_dataplayer STATIC)
add_library(YARP::YARP_dataplayer ALIAS YARP_dataplayer)

set(YARP_dataplayer_HDRS yarp/dataplayer/YarpDataplayer.h)

set(YARP_dataplayer_IMPL_HDRS )

set(YARP_dataplayer_SRCS yarp/dataplayer/YarpDataplayer.cpp)

source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Source Files"
  FILES ${YARP_dataplayer_SRCS}
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Header Files"
  FILES
    ${YARP_dataplayer_HDRS}
    ${YARP_dataplayer_IMPL_HDRS}
)

target_sources(YARP_dataplayer
  PRIVATE
    ${YARP_dataplayer_SRCS}
    ${YARP_dataplayer_HDRS}
    ${YARP_dataplayer_IMPL_HDRS}
)

target_include_directories(YARP_dataplayer
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if(MSVC)
  target_include_directories(YARP_dataplayer SYSTEM PRIVATE ${dirent_INCLUDE_DIRS})
endif()

target_compile_features(YARP_dataplayer PUBLIC cxx_std_14)

target_link_libraries(YARP_dataplayer
  PUBLIC
    YARP::YARP_os
  PRIVATE
    YARP::YARP_sig
    YARP::YARP_init
    YARP::YARP_rosmsg
)
list(APPEND YARP_dataplayer_PUBLIC_DEPS
  YARP_os
)
list(APPEND YARP_dataplayer_PRIVATE_DEPS
  YARP_sig
  YARP_init
  YARP_rosmsg
)


set_property(TARGET YARP_dataplayer PROPERTY PUBLIC_HEADER ${YARP_dataplayer_HDRS})
set_property(TARGET YARP_dataplayer PROPERTY PRIVATE_HEADER ${YARP_dataplayer_IMPL_HDRS})
set_property(TARGET YARP_dataplayer PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_dataplayer PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_dataplayer PROPERTY FOLDER "Libraries/Private")


install(
  TARGETS YARP_dataplayer
  EXPORT YARP_dataplayer
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT YARP_dataplayer
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_dataplayer
    NAMELINK_COMPONENT YARP_dataplayer-dev
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_dataplayer-dev
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dataplayer"
    COMPONENT YARP_dataplayer-dev
  PRIVATE_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dataplayer/impl"
    COMPONENT YARP_dataplayer-priv-dev
)


set(YARP_dataplayer_PUBLIC_DEPS ${YARP_dataplayer_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_dataplayer_PRIVATE_DEPS ${YARP_dataplayer_PRIVATE_DEPS} PARENT_SCOPE)
