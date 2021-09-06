# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os dev REQUIRED)

yarp_begin_plugin_library(foobaz)
add_subdirectory(baz)
add_subdirectory(foo)
yarp_end_plugin_library(foobaz)
yarp_add_plugin_yarpdev_executable(foobazdev foobaz)
