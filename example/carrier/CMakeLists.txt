# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)
project(Carrier)

find_package(YARP REQUIRED)

add_executable(carrier_stub carrier_stub.cpp)
target_link_libraries(carrier_stub ${YARP_LIBRARIES})

add_executable(carrier_human carrier_human.cpp)
target_link_libraries(carrier_human ${YARP_LIBRARIES})
