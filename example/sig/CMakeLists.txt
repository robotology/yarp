# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS sig REQUIRED)

add_executable(wav_test)
target_sources(wav_test PRIVATE wav_test.cpp)
target_link_libraries(wav_test PRIVATE ${YARP_LIBRARIES})
