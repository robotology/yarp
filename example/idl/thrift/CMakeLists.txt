# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(thrift_example)

find_package(YARP COMPONENTS os idl_tools REQUIRED)
set(CMAKE_CXX_FLAGS "-Weverything -Wno-c++98-compat -Wno-padded -Wno-shadow")

set(THRIFT_FILES thrift/firstInterface/PointD.thrift
                 thrift/secondInterface/demo.thrift)
yarp_idl_to_dir(INPUT_FILES ${THRIFT_FILES}
                OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/generated"
                SOURCES_VAR THRIFT_SOURCES
                HEADERS_VAR THRIFT_HEADERS
                INCLUDE_DIRS_VAR THRIFT_INCLUDE_DIRS)

message(STATUS "Thrift files: ${THRIFT_FILES}")
message(STATUS "Thrift headers: ${THRIFT_HEADERS}")
message(STATUS "Thrift sources: ${THRIFT_SOURCES}")
message(STATUS "Thrift include dirs: ${THRIFT_INCLUDE_DIRS}")

add_library(generatedDemoLibrary)
target_sources(generatedDemoLibrary PRIVATE ${THRIFT_HEADERS}
                                            ${THRIFT_SOURCES}
                                            ${THRIFT_FILES})
target_include_directories(generatedDemoLibrary PUBLIC $<BUILD_INTERFACE:${THRIFT_INCLUDE_DIRS}>)

target_link_libraries(generatedDemoLibrary YARP::YARP_os)

add_subdirectory(userImpl)
