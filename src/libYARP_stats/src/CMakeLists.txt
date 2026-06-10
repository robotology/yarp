# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_stats)
add_library(YARP::YARP_stats ALIAS YARP_stats)

set(YARP_stats_HDRS
  yarp/stats/Stats.h
)

set(YARP_stats_SRCS
  yarp/stats/Stats.cpp
)

# Handle the YARP thrift messages
include(YarpChooseIDL)

set(YARP_stats_IDL
  idl/YarpStatsMsgs.thrift
)
yarp_choose_idl(YARP_stats_idl ${YARP_stats_IDL})

unset(YARP_stats_idl_HDRS)
unset(YARP_stats_idl_IMPL_HDRS)
foreach(_f IN LISTS YARP_stats_idl_GEN_FILES)
  if("${_f}" MATCHES "\\.h$")
    if ("${_f}" MATCHES "yarp/stats" AND NOT "${_f}" MATCHES "yarp/stats/impl")
      list(APPEND YARP_stats_idl_HDRS "${_f}")
    else()
      list(APPEND YARP_stats_idl_IMPL_HDRS ${_f})
    endif()
  endif()
endforeach()

#message ("YARP_stats_idl_HDRS: ${YARP_stats_idl_HDRS}")
#message ("YARP_stats_idl_GEN_FILES: ${YARP_stats_idl_GEN_FILES}")
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Source Files"
  FILES
    ${YARP_stats_SRCS}
)
source_group(
  TREE "${CMAKE_CURRENT_SOURCE_DIR}"
  PREFIX "Header Files"
  FILES
    ${YARP_stats_HDRS}
)

source_group(
  TREE "${YARP_stats_idl_BUILD_INTERFACE_INCLUDE_DIRS}"
  PREFIX "Generated Files"
  FILES ${YARP_stats_idl_GEN_FILES}
)

target_sources(YARP_stats
  PRIVATE
    ${YARP_stats_SRCS}
    ${YARP_stats_HDRS}
    ${YARP_stats_idl_GEN_FILES}
)

target_include_directories(YARP_stats
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${YARP_stats_idl_BUILD_INTERFACE_INCLUDE_DIRS}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(YARP_stats PUBLIC cxx_std_17)

target_link_libraries(YARP_stats PRIVATE YARP::YARP_os)
list(APPEND YARP_stats_PRIVATE_DEPS YARP_os)

set_property(TARGET YARP_stats PROPERTY PUBLIC_HEADER ${YARP_stats_HDRS} ${YARP_stats_idl_HDRS})
set_property(TARGET YARP_stats PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_stats PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_stats PROPERTY FOLDER "Libraries/Private")


install(
  TARGETS YARP_stats
  EXPORT YARP_stats
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT YARP_stats
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_stats
    NAMELINK_COMPONENT YARP_stats-dev
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT YARP_stats-dev
  PUBLIC_HEADER
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/stats"
    COMPONENT YARP_stats-dev)

set(YARP_stats_PUBLIC_DEPS ${YARP_stats_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_stats_PRIVATE_DEPS ${YARP_stats_PRIVATE_DEPS} PARENT_SCOPE)
