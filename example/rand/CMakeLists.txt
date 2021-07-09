# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(Rand)

find_package(YARP COMPONENTS sig math REQUIRED)

add_executable(rand)
target_sources(rand PRIVATE rand.cpp)
target_link_libraries(rand PRIVATE ${YARP_LIBRARIES})
