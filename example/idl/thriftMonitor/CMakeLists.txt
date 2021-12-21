# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# //! [CMakeLists.txt Protocol Library]
cmake_minimum_required(VERSION 3.16)
project(thriftEditor_example)

# Needed to link the static library from the plugin, on 64 bit
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(YARP COMPONENTS os idl_tools REQUIRED)

add_library(demo_protocol STATIC)

yarp_idl_to_dir(
  INPUT_FILES Demo.thrift
  OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/demo
  SOURCES_VAR IDL_GEN_SRCS
  HEADERS_VAR IDL_GEN_HDRS
  INCLUDE_DIRS_VAR IDL_INCLUDE_DIRS
  PLACEMENT MERGED
)

target_sources(demo_protocol
  PRIVATE
    ${IDL_GEN_SRCS}
    ${IDL_GEN_HDRS}
)

target_include_directories(demo_protocol
  PUBLIC
    $<BUILD_INTERFACE:${IDL_INCLUDE_DIRS}>
)

target_link_libraries(demo_protocol
  PRIVATE
    YARP::YARP_os)
# //! [CMakeLists.txt Protocol Library]


# //! [CMakeLists.txt Client]
add_executable(demo_client)
target_sources(demo_client PRIVATE DemoClient.cpp)
target_link_libraries(demo_client
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    demo_protocol
)
# //! [CMakeLists.txt Client]


# //! [CMakeLists.txt Server]
add_executable(demo_server)
target_sources(demo_server PRIVATE DemoServer.cpp)
target_link_libraries(demo_server
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    demo_protocol
)
# //! [CMakeLists.txt Server]


# //! [CMakeLists.txt Monitor]
yarp_prepare_plugin(demo_monitor
  TYPE Demo::Monitor
  INCLUDE ${IDL_GEN_HDRS} # FIXME there should be a way to get this automatically
  CATEGORY portmonitor
  INTERNAL ON
)

yarp_add_plugin(demo_monitor)

target_link_libraries(demo_monitor
  PRIVATE
    YARP::YARP_os
    demo_protocol
)

yarp_install(
  TARGETS demo_monitor
  EXPORT demo_monitor
  COMPONENT demo_monitor
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
# //! [CMakeLists.txt Monitor]
