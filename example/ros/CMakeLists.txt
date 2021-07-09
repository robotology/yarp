# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)
project(yarp_ros_example)

find_package(YARP 3.3.103 COMPONENTS os dev math rosmsg idl_tools REQUIRED)

add_executable(add_int_client_v1)
target_sources(add_int_client_v1 PRIVATE add_int_client_v1.cpp)
target_link_libraries(add_int_client_v1 PRIVATE YARP::YARP_os
                                                YARP::YARP_init
                                                YARP::YARP_rosmsg)

add_executable(add_int_client_v2)
target_sources(add_int_client_v2 PRIVATE add_int_client_v2.cpp)
target_link_libraries(add_int_client_v2 PRIVATE YARP::YARP_os
                                                YARP::YARP_init
                                                YARP::YARP_rosmsg)

add_executable(add_int_server_v1)
target_sources(add_int_server_v1 PRIVATE add_int_server_v1.cpp)
target_link_libraries(add_int_server_v1 PRIVATE YARP::YARP_os
                                                YARP::YARP_init
                                                YARP::YARP_rosmsg)

add_executable(listener_v1)
target_sources(listener_v1 PRIVATE listener_v1.cpp)
target_link_libraries(listener_v1 PRIVATE YARP::YARP_os
                                          YARP::YARP_init
                                          YARP::YARP_rosmsg)

add_executable(listener_v2)
target_sources(listener_v2 PRIVATE listener_v2.cpp)
target_link_libraries(listener_v2 PRIVATE YARP::YARP_os
                                          YARP::YARP_init
                                          YARP::YARP_rosmsg)

add_executable(listener)
target_sources(listener PRIVATE listener.cpp)
target_link_libraries(listener PRIVATE YARP::YARP_os
                                       YARP::YARP_init
                                       YARP::YARP_rosmsg)

add_executable(talker)
target_sources(talker PRIVATE talker.cpp)
target_link_libraries(talker PRIVATE YARP::YARP_os
                                     YARP::YARP_init
                                     YARP::YARP_rosmsg)

add_executable(rosmap_publisher)
target_sources(rosmap_publisher PRIVATE rosmap_publisher.cpp)
target_link_libraries(rosmap_publisher PRIVATE
                                        YARP::YARP_os
                                        YARP::YARP_dev
                                        YARP::YARP_math
                                        YARP::YARP_init
                                        YARP::YARP_rosmsg)


yarp_idl_to_dir(INPUT_FILES package/src/yarp_test/srv/AddTwoInts.srv
                OUTPUT_DIR ${CMAKE_BINARY_DIR}/msg
                SOURCES_VAR AddTwoInts_SOURCES
                HEADERS_VAR AddTwoInts_HEADERS
                INCLUDE_DIRS_VAR AddTwoInts_INCLUDES)

add_executable(add_int_server_v1b)
target_sources(add_int_server_v1b PRIVATE add_int_server_v1b.cpp
                                          ${AddTwoInts_SOURCES}
                                          ${AddTwoInts_HEADERS})
target_include_directories(add_int_server_v1b PRIVATE ${AddTwoInts_INCLUDES})
target_link_libraries(add_int_server_v1b PRIVATE YARP::YARP_os
                                                 YARP::YARP_init
                                                 YARP::YARP_rosmsg)

add_executable(add_int_client_v1b)
target_sources(add_int_client_v1b PRIVATE add_int_client_v1b.cpp
                                          ${AddTwoInts_SOURCES}
                                          ${AddTwoInts_HEADERS})
target_include_directories(add_int_client_v1b PRIVATE ${AddTwoInts_INCLUDES})
target_link_libraries(add_int_client_v1b PRIVATE YARP::YARP_os
                                                 YARP::YARP_init
                                                 YARP::YARP_rosmsg)
