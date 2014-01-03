# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Francesco Nori
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

ADD_EXECUTABLE(sound_receiver sound_receiver.cpp)
ADD_EXECUTABLE(sound_sender_mic sound_sender_mic.cpp)
ADD_EXECUTABLE(sound_sender_file sound_sender_file.cpp)
ADD_EXECUTABLE(sound_sender_file_chunks sound_sender_file_chunks.cpp)
ADD_EXECUTABLE(sound_sender_file_sum sound_sender_file_sum.cpp)
ADD_EXECUTABLE(sound_loopback sound_loopback.cpp)

TARGET_LINK_LIBRARIES(sound_loopback ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(sound_sender_mic ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(sound_sender_file ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(sound_sender_file_chunks ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(sound_sender_file_sum ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(sound_receiver ${YARP_LIBRARIES})
