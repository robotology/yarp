# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

project(gtkdemo)

find_package(YARP REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})
link_libraries(${YARP_LIBRARIES})
include_directories(${YARP_INCLUDE_DIRS})

find_package(GtkPlus REQUIRED)
add_definitions(${GTKPLUS_C_FLAGS})
link_libraries(${GTKPLUS_LINK_FLAGS})
include_directories(${GTKPLUS_INCLUDE_DIR})

file(GLOB folder_source *.cpp)
file(GLOB folder_header *.h include/*.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

add_executable(gtkdemo ${folder_source} ${folder_header})
