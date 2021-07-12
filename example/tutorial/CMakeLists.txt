# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# Make sure that YARP_DIR is set in your environment to wherever you
# compiled YARP.

project(run_fakebot)
cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os dev REQUIRED)

set(ENABLE_fakebot TRUE CACHE BOOLEAN "we want to use fakebot")

# Search for source code.
set(folder_source fakebot/FakeBot.cpp
                  adder.cpp
                  add_fakebot.cpp
                  yarpdev.cpp)
set(folder_header fakebot/FakeBot.h
                  adder.h)

# Set up our main executable.
add_executable(run_fakebot)
target_sources(run_fakebot PRIVATE ${folder_source} ${folder_header})
target_include_directories(run_fakebot PRIVATE ${CMAKE_SOURCE_DIR}/fakebot)
target_link_libraries(run_fakebot PRIVATE ${YARP_LIBRARIES})
