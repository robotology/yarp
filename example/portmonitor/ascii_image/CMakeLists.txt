# Copyright (C) 2014 iCub Facility
# Authors: Ali Paikan
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

FIND_PACKAGE(YARP REQUIRED)
LINK_LIBRARIES(${YARP_LIBRARIES})
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

ADD_LIBRARY(ascii_image SHARED AsciiImage.cpp AsciiImage.h)

