# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

# Copyright: (C) 2010 Renaud Detry
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LICENSE

cmake_minimum_required(VERSION 3.0)
project(yayarpview)

# replace with the path to your YARP binaries if needed
set(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

find_package(YARP REQUIRED)

find_package(X11)
include_directories(${X11_INCLUDE_DIR})

# add_definitions("-Dcimg_use_png -Dcimg_use_jpeg")

file(GLOB HEADERS *.h)
file(GLOB CODE *.cpp)

add_executable(yayarpview ${HEADERS} ${CODE})

target_link_libraries(yayarpview ${YARP_LIBRARIES}
                                 ${X11_LIBRARIES}
                                 -lpthread
                                 -lm)
# target_link_libraries("yayarpview -ljpeg -lpng -lpthread -lm")
