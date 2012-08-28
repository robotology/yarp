# Copyright: (C) 2012 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 2.8)

find_package(YARP REQUIRED)
add_subdirectory(thrift)
add_subdirectory(generated)
add_subdirectory(userImpl)

