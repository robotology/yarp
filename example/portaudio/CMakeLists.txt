# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Francesco Nori
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.6.4)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

ADD_EXECUTABLE(sound_receiver sound_receiver.cpp)
ADD_EXECUTABLE(sound_sender sound_sender.cpp)
ADD_EXECUTABLE(sound_loopback sound_loopback.cpp)

TARGET_LINK_LIBRARIES(sound_loopback ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(sound_sender ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(sound_receiver ${YARP_LIBRARIES})
