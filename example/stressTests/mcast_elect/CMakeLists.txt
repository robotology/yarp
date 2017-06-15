# Copyright: (C) 2010 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

# Start a project.
project(mcast_elect)

# Find YARP.  Point the YARP_DIR environment variable at your build.
find_package(YARP REQUIRED)

# Search for source code.
file(GLOB folder_source *.cpp *.cc *.c)
file(GLOB folder_header *.h)

# Automatically add include directories if needed.
foreach(header_file ${folder_header})
  get_filename_component(p ${header_file} PATH)
  include_directories(${p})
endforeach(header_file ${folder_header})

# Set up our main executable.
if (folder_source)
  add_executable(mcast_elect ${folder_source} ${folder_header})
  target_link_libraries(mcast_elect ${YARP_LIBRARIES})
else (folder_source)
  message(FATAL_ERROR "No source code files found. Please add something")
endif (folder_source)
