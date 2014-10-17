# Copyright: (C) 2011 IITRBCS
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

enable_testing()

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpIDL)

set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}/bits")
yarp_idl_to_dir(Demo.msg ${generated_libs_dir} S1 H1)
yarp_idl_to_dir(Tennis.srv ${generated_libs_dir} S2 H3)
yarp_idl_to_dir(Rpc.srv ${generated_libs_dir} S3 H3)
yarp_idl_to_dir(SharedData.msg ${generated_libs_dir} S4 H4)
yarp_idl_to_dir(HeaderTest.msg ${generated_libs_dir} S5 H5)
yarp_idl_to_dir(HeaderTest2.srv ${generated_libs_dir} S6 H6)

include_directories(${generated_libs_dir}/include)
include_directories(${YARP_INCLUDE_DIRS})
add_executable(demo_test main.cpp ${S1} ${S2} ${S3} ${S4} ${H1} ${H2} ${H3} ${H4} ${S5} ${H5} ${S6} ${H6})
target_link_libraries(demo_test ${YARP_LIBRARIES})

add_test(demo_basic demo_test --help)


