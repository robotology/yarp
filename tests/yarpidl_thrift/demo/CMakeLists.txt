# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)
project(demo)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS 1)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Autogen Targets")
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")

find_package(YARP REQUIRED)

# ACE is required for yarp/os/impl/UnitTests.h
find_package(ACE REQUIRED)
include_directories(${ACE_INCLUDE_DIRS})

# Test using yarp_idl_to_dir
set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}")
yarp_idl_to_dir(demo.thrift ${generated_libs_dir} S1 H1 I1)
yarp_idl_to_dir(namespaced.thrift ${generated_libs_dir} S2 H2 I2)
yarp_idl_to_dir(objects3D.thrift ${generated_libs_dir} S3 H3 I3)
yarp_idl_to_dir(wrapping.thrift ${generated_libs_dir} S4 H4 I4)
yarp_idl_to_dir(sub/directory/clock_rpc.thrift ${generated_libs_dir} S5 H5 I5)
yarp_idl_to_dir(settings.thrift ${generated_libs_dir} S6 H6 I6)

include_directories(${generated_libs_dir}/include)
add_executable(demo_yarp_idl_to_dir main.cpp ${S1} ${S2} ${S3} ${S4} ${S5} ${S6} ${H1} ${H2} ${H3} ${H4} ${H5} ${H6})
target_link_libraries(demo_yarp_idl_to_dir ${YARP_LIBRARIES} ${ACE_LIBRARIES})

add_test(NAME demo_yarp_idl_to_dir
         COMMAND demo_yarp_idl_to_dir)


# Test using yarp_add_idl
set(IDL_FILES demo.thrift
              namespaced.thrift
              objects3D.thrift
              wrapping.thrift
              sub/directory/clock_rpc.thrift
              settings.thrift)
yarp_add_idl(IDL_GEN_FILES ${IDL_FILES})
add_executable(demo_yarp_add_idl main.cpp ${IDL_GEN_FILES})
target_link_libraries(demo_yarp_add_idl ${YARP_LIBRARIES} ${ACE_LIBRARIES})

add_test(NAME demo_yarp_add_idl
         COMMAND demo_yarp_add_idl)
