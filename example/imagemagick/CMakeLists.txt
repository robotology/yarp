# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(magicktest)

find_package(YARP REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

include_directories(${ImageMagick_INCLUDE_DIRS})

add_executable(magicktest main.cpp)

target_link_libraries(magicktest ${ImageMagick_LIBRARIES}
                          ${YARP_LIBRARIES})
