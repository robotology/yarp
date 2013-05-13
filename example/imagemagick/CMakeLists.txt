# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

PROJECT(magicktest)

FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(ImageMagick COMPONENTS Magick++ REQUIRED)

INCLUDE_DIRECTORIES(${ImageMagick_INCLUDE_DIRS})
LINK_LIBRARIES(${ImageMagick_LIBRARIES})

INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

ADD_EXECUTABLE(magicktest main.cpp)
