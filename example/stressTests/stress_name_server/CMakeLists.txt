# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os REQUIRED)

add_executable(stress_name_server_topics)
target_sources(stress_name_server_topics PRIVATE stress_name_server_topics.cpp)
target_link_libraries(stress_name_server_topics PRIVATE ${YARP_LIBRARIES})

add_executable(stress_name_server_reg)
target_sources(stress_name_server_reg PRIVATE stress_name_server_reg.cpp)
target_link_libraries(stress_name_server_reg PRIVATE ${YARP_LIBRARIES})
