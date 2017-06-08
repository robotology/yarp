# Copyright: (C) 2011 IITRBCS
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(demo)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS 1)
set_property(GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "Autogen Targets")
set_property(GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP "Generated Files")

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include_directories(${YARP_INCLUDE_DIRS})

# ACE is required for yarp/os/impl/UnitTests.h
find_package(ACE REQUIRED)
include_directories(${ACE_INCLUDE_DIRS})

include(YarpIDL)

# Test using yarp_idl_to_dir
set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}/bits")
yarp_idl_to_dir(SharedData.msg ${generated_libs_dir} S1 H1)
yarp_idl_to_dir(Demo.msg ${generated_libs_dir} S2 H2)
yarp_idl_to_dir(Tennis.srv ${generated_libs_dir} S3 H3)
yarp_idl_to_dir(Rpc.srv ${generated_libs_dir} S4 H4)
yarp_idl_to_dir(HeaderTest.msg ${generated_libs_dir} S5 H5)
yarp_idl_to_dir(HeaderTest2.srv ${generated_libs_dir} S6 H6)

include_directories(${generated_libs_dir}/include)
add_executable(demo_yarp_idl_to_dir main.cpp ${S1} ${S2} ${S3} ${S4} ${H1} ${H2} ${H3} ${H4} ${S5} ${H5} ${S6} ${H6})
target_link_libraries(demo_yarp_idl_to_dir ${YARP_LIBRARIES} ${ACE_LIBRARIES})

add_test(NAME demo_yarp_idl_to_dir
         COMMAND demo_yarp_idl_to_dir --help)

# Test using yarp_add_idl
set(IDL_FILES SharedData.msg
              Demo.msg
              Tennis.srv
              Rpc.srv
              HeaderTest.msg
              HeaderTest2.srv)
yarp_add_idl(IDL_GEN_FILES ${IDL_FILES})

add_executable(demo_yarp_add_idl main.cpp ${IDL_GEN_FILES})
target_link_libraries(demo_yarp_add_idl ${YARP_LIBRARIES} ${ACE_LIBRARIES})

add_test(NAME demo_yarp_add_idl
         COMMAND demo_yarp_add_idl --help)
