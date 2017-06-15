# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(magicktest)

find_package(YARP REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)

include_directories(${ImageMagick_INCLUDE_DIRS})

add_executable(magicktest main.cpp)

target_link_libraries(magicktest ${ImageMagick_LIBRARIES}
                          ${YARP_LIBRARIES})
