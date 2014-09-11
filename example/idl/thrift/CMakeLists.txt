# Copyright: (C) 2012 Robotics Brain and Cognitive Sciences, Istituto Italiano di Tecnologia
# Authors: Elena Ceseracciu
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

find_package(YARP REQUIRED)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
add_subdirectory(thrift)
add_subdirectory(generated)
add_subdirectory(userImpl)

