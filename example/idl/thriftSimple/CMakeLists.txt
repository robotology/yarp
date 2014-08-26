# Copyright: (C) 2012 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
# Authors: Elena Ceseracciu
# CopyPolicy:  Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

#find YARP
find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
include(YarpIDL)

#compile definition file to generate source code into the desired directory
set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
yarp_idl_to_dir(demo.thrift ${generated_libs_dir})

# CMake has compiled the file during the configuration process, so now we can include the generated .cmake file that lists all sources into "headers" and "sources" variables
include(demo_thrift.cmake)
include_directories(${generated_libs_dir}/include ${YARP_INCLUDE_DIRS})

# Create the server executable that implements the interface
add_executable(demoServer DemoServer.cpp ${headers} ${sources})
TARGET_LINK_LIBRARIES(demoServer ${YARP_LIBRARIES})

# Create a fancier YARP-module server that implements the interface
add_executable(demoServerModule DemoServerModule.cpp ${headers} ${sources})
TARGET_LINK_LIBRARIES(demoServerModule ${YARP_LIBRARIES})

# Create a test client executable
add_executable(demoClient DemoClient.cpp ${headers} ${sources})
TARGET_LINK_LIBRARIES(demoClient ${YARP_LIBRARIES})

