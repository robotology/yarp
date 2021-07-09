# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(stress_connect)
target_sources(stress_connect PRIVATE stress_connect.cpp)
target_link_libraries(stress_connect PRIVATE ${YARP_LIBRARIES})
