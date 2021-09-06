# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(opencv_test)

find_package(YARP REQUIRED COMPONENTS sig cv)
find_package(OpenCV REQUIRED)

add_executable(opencv_test)
target_sources(opencv_test PRIVATE main.cpp)
target_link_libraries(opencv_test PRIVATE ${YARP_LIBRARIES} ${OpenCV_LIBRARIES})
