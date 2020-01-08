# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(demo)
enable_testing()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS 1)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Autogen Targets")
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")

find_package(YARP COMPONENTS os sig idl_tools REQUIRED)

include(ParseAndAddCatchTests)
set(PARSE_CATCH_TESTS_VERBOSE OFF CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_NO_HIDDEN_TESTS ON CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_ADD_TARGET_IN_TEST_NAME ON CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_ADD_FIXTURE_IN_TEST_NAME ON CACHE INTERNAL "")
set(PARSE_CATCH_TESTS_ADD_TO_CONFIGURE_DEPENDS ON CACHE INTERNAL "")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../extern/catch2)
set(AdditionalCatchParameters "-s" "--use-colour" "yes")
set(OptionalCatchTestLauncher ${YARP_TEST_LAUNCHER})

set(IDL_FILES demo.thrift
              namespaced.thrift
              objects3D.thrift
              wrapping.thrift
              sub/directory/clock_rpc.thrift
              settings.thrift)

# Test using yarp_idl_to_dir
set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}/yarp_idl_to_dir")
yarp_idl_to_dir(INPUT_FILES ${IDL_FILES}
                OUTPUT_DIR "${generated_libs_dir}"
                SOURCES_VAR IDL_GEN_SRCS
                HEADERS_VAR IDL_GEN_HDRS
                INCLUDE_DIRS_VAR IDL_INCLUDE_DIRS
                THRIFT_INCLUDE_PREFIX
                THRIFT_NO_NAMESPACE_PREFIX)

add_executable(demo_yarp_idl_to_dir)
target_compile_definitions(demo_yarp_idl_to_dir PRIVATE THRIFT_INCLUDE_PREFIX
                                                        THRIFT_NO_NAMESPACE_PREFIX)
target_sources(demo_yarp_idl_to_dir PRIVATE main.cpp
                                            ${IDL_GEN_SRCS}
                                            ${IDL_GEN_HDRS})
target_include_directories(demo_yarp_idl_to_dir PRIVATE ${generated_libs_dir}/include)
target_link_libraries(demo_yarp_idl_to_dir PRIVATE YARP::YARP_os
                                                   YARP::YARP_init
                                                   YARP::YARP_sig)

ParseAndAddCatchTests(demo_yarp_idl_to_dir)


# Test using yarp_add_idl
yarp_add_idl(IDL_GEN_FILES ${IDL_FILES})
add_executable(demo_yarp_add_idl)
target_compile_definitions(demo_yarp_add_idl PRIVATE THRIFT_INCLUDE_PREFIX
                                                     THRIFT_NO_NAMESPACE_PREFIX)
target_sources(demo_yarp_add_idl PRIVATE main.cpp
                                         ${IDL_GEN_FILES})
target_link_libraries(demo_yarp_add_idl PRIVATE YARP::YARP_os
                                                YARP::YARP_init
                                                YARP::YARP_sig)

ParseAndAddCatchTests(demo_yarp_add_idl)
