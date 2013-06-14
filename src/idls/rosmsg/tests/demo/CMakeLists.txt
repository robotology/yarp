# Copyright: (C) 2011 IITRBCS
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

enable_testing()

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpIDL)

set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}")
yarp_idl_to_dir(Demo.msg ${generated_libs_dir})
yarp_idl_to_dir(Tennis.srv ${generated_libs_dir})

include_directories(${generated_libs_dir}/demo)
include_directories(${generated_libs_dir}/tennis)
include_directories(${YARP_INCLUDE_DIRS})
add_executable(demo_test main.cpp ${generated_libs_dir}/demo/Demo.h ${generated_libs_dir}/tennis/Tennis.h ${generated_libs_dir}/tennis/TennisReply.h)
target_link_libraries(demo_test ${YARP_LIBRARIES})

add_test(demo_basic demo_test --help)


