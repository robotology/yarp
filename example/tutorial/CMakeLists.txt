# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 3.0)

# Start a project.
project(run_fakebot)

find_package(YARP REQUIRED)

set(ENABLE_fakebot TRUE CACHE BOOLEAN "we want to use fakebot")

# Search for source code.
set(folder_source fakebot/FakeBot.cpp
                  adder.cpp
                  add_fakebot.cpp
                  yarpdev.cpp)
set(folder_header fakebot/FakeBot.h
                  adder.h)
source_group("Source Files" FILES ${folder_source})
source_group("Header Files" FILES ${folder_header})

include_directories(${CMAKE_SOURCE_DIR}/fakebot)

# Set up our main executable.
if(folder_source)
  add_executable(run_fakebot ${folder_source} ${folder_header})
  target_link_libraries(run_fakebot ${YARP_LIBRARIES})
else()
  message(FATAL_ERROR "No source code files found. Please add something")
endif()
