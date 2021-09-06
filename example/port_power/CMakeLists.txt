# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

cmake_minimum_required(VERSION 3.12)
project(PortPower)

find_package(YARP COMPONENTS os REQUIRED)

file(GLOB_RECURSE examples *.cpp)

foreach(example ${examples})
  # get name without extension
  get_filename_component(example_we "${example}" NAME_WE)
  add_executable(${example_we})
  target_sources(${example_we} PRIVATE ${example})
  target_include_directories(${example_we} PRIVATE ${PROJECT_SOURCE_DIR})
  target_link_libraries(${example_we} PRIVATE ${YARP_LIBRARIES})
endforeach(example ${examples})
