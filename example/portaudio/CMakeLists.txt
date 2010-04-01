# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Francesco Nori
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

ADD_EXECUTABLE(sound_receiver sound_receiver.cpp)
ADD_EXECUTABLE(sound_sender sound_sender.cpp)
ADD_EXECUTABLE(sound_loopback sound_loopback.cpp)
