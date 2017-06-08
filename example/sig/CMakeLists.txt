# Copyright: (C) 2010 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

find_package(YARP REQUIRED)

add_executable(wav_test wav_test.cpp)
target_link_libraries(wav_test ${YARP_LIBRARIES})
