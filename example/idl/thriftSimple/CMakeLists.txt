# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(thriftSimple_example)

set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -Wno-padded")

find_package(YARP COMPONENTS os idl_tools REQUIRED)

#compile definition file to generate source code into the desired directory
yarp_idl_to_dir(INPUT_FILES demo.thrift
                OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}"
                SOURCES_VAR THRIFT_SOURCES
                HEADERS_VAR THRIFT_HEADERS
                INCLUDE_DIRS_VAR THRIFT_INCLUDE_DIRS
                VERBOSE)

# Create the server executable that implements the interface
add_executable(demoServer)
target_sources(demoServer PRIVATE DemoServer.cpp
                                  ${THRIFT_SOURCES}
                                  ${THRIFT_HEADERS})
target_include_directories(demoServer PRIVATE ${THRIFT_INCLUDE_DIRS})
target_link_libraries(demoServer PRIVATE YARP::YARP_os
                                         YARP::YARP_init)

# Create a fancier YARP-module server that implements the interface
add_executable(demoServerModule)
target_sources(demoServerModule PRIVATE DemoServerModule.cpp
                                        ${THRIFT_SOURCES}
                                        ${THRIFT_HEADERS})
target_include_directories(demoServerModule PRIVATE ${THRIFT_INCLUDE_DIRS})
target_link_libraries(demoServerModule PRIVATE YARP::YARP_os
                                               YARP::YARP_init)

# Create a test client executable
add_executable(demoClient)
target_sources(demoClient PRIVATE DemoClient.cpp
                                  ${THRIFT_SOURCES}
                                  ${THRIFT_HEADERS})
target_include_directories(demoClient PRIVATE ${THRIFT_INCLUDE_DIRS})
target_link_libraries(demoClient PRIVATE YARP::YARP_os
                                         YARP::YARP_init)
