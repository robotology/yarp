# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)

#find YARP
find_package(YARP REQUIRED)

#compile definition file to generate source code into the desired directory
set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
yarp_idl_to_dir(demo.thrift ${generated_libs_dir})

# CMake has compiled the file during the configuration process, so now we can include the generated .cmake file that lists all sources into "headers" and "sources" variables
include(${generated_libs_dir}/demo_thrift.cmake)
include_directories(${generated_libs_dir}/include)

# Create the server executable that implements the interface
add_executable(demoServer DemoServer.cpp ${headers} ${sources})
target_link_libraries(demoServer ${YARP_LIBRARIES})

# Create a fancier YARP-module server that implements the interface
add_executable(demoServerModule DemoServerModule.cpp ${headers} ${sources})
target_link_libraries(demoServerModule ${YARP_LIBRARIES})

# Create a test client executable
add_executable(demoClient DemoClient.cpp ${headers} ${sources})
target_link_libraries(demoClient ${YARP_LIBRARIES})

