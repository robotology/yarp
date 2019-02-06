# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)

find_package(YARP REQUIRED)

add_executable(sound_receiver sound_receiver.cpp)
target_link_libraries(sound_receiver ${YARP_LIBRARIES})

add_executable(sound_sender_mic sound_sender_mic.cpp)
target_link_libraries(sound_sender_mic ${YARP_LIBRARIES})

add_executable(sound_sender_file sound_sender_file.cpp)
target_link_libraries(sound_sender_file ${YARP_LIBRARIES})

add_executable(sound_sender_file_chunks sound_sender_file_chunks.cpp)
target_link_libraries(sound_sender_file_chunks ${YARP_LIBRARIES})

add_executable(sound_sender_file_sum sound_sender_file_sum.cpp)
target_link_libraries(sound_sender_file_sum ${YARP_LIBRARIES})

add_executable(sound_loopback sound_loopback.cpp)
target_link_libraries(sound_loopback ${YARP_LIBRARIES})
