# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP REQUIRED)

add_executable(add_int_client_v1)
target_sources(add_int_client_v1 PRIVATE add_int_client_v1.cpp)
target_link_libraries(add_int_client_v1 PRIVATE ${YARP_LIBRARIES})

add_executable(add_int_client_v2)
target_sources(add_int_client_v2 PRIVATE add_int_client_v2.cpp)
target_link_libraries(add_int_client_v2 PRIVATE ${YARP_LIBRARIES})

add_executable(add_int_server_v1)
target_sources(add_int_server_v1 PRIVATE add_int_server_v1.cpp)
target_link_libraries(add_int_server_v1 PRIVATE ${YARP_LIBRARIES})

add_executable(listener_v1)
target_sources(listener_v1 PRIVATE listener_v1.cpp)
target_link_libraries(listener_v1 PRIVATE ${YARP_LIBRARIES})

add_executable(listener_v2)
target_sources(listener_v2 PRIVATE listener_v2.cpp)
target_link_libraries(listener_v2 PRIVATE ${YARP_LIBRARIES})

add_executable(listener)
target_sources(listener PRIVATE listener.cpp)
target_link_libraries(listener PRIVATE ${YARP_LIBRARIES})

add_executable(talker)
target_sources(talker PRIVATE talker.cpp)
target_link_libraries(talker PRIVATE ${YARP_LIBRARIES})

yarp_idl_to_dir(package/src/yarp_test/srv/AddTwoInts.srv ${CMAKE_BINARY_DIR}/msg AddTwoInts_SOURCES AddTwoInts_HEADERS AddTwoInts_INCLUDES)
yarp_idl_to_dir(std_msgs/String.msg ${CMAKE_BINARY_DIR}/msg String_SOURCES String_HEADERS String_INCLUDES)
set(SOURCES ${AddTwoInts_SOURCES} ${String_SOURCES})
set(HEADERS ${AddTwoInts_HEADERS} ${String_HEADERS})
set(INCLUDES ${AddTwoInts_INCLUDES} ${String_INCLUDES})

add_executable(add_int_server_v1b)
target_sources(add_int_server_v1b PRIVATE add_int_server_v1b.cpp ${SOURCES} ${HEADERS})
target_include_directories(add_int_server_v1b PRIVATE ${INCLUDES})
target_link_libraries(add_int_server_v1b PRIVATE ${YARP_LIBRARIES})

add_executable(add_int_client_v1b)
target_sources(add_int_client_v1b PRIVATE add_int_client_v1b.cpp ${SOURCES} ${HEADERS})
target_include_directories(add_int_client_v1b PRIVATE ${INCLUDES})
target_link_libraries(add_int_client_v1b PRIVATE ${YARP_LIBRARIES})
