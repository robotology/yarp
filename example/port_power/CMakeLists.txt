# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

cmake_minimum_required(VERSION 3.0)
project(PortPower)

find_package(YARP REQUIRED)

file(GLOB_RECURSE examples *.cpp)

include_directories(${PROJECT_SOURCE_DIR})

foreach(example ${examples})
  # get name without extension
  get_filename_component(example_we "${example}" NAME_WE)
  add_executable(${example_we} ${example})
  target_link_libraries(${example_we} ${YARP_LIBRARIES})
endforeach(example ${examples})
