# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(Examples)

# Be sure to set YARP_DIR environment variable if YARP is not installed
# somewhere findable.
find_package(YARP REQUIRED)

add_executable(bottle_add bottle_add.cpp)
target_link_libraries(bottle_add ${YARP_LIBRARIES})

add_executable(simple_sender simple_sender.cpp)
target_link_libraries(simple_sender ${YARP_LIBRARIES})

add_executable(simple_receiver simple_receiver.cpp)
target_link_libraries(simple_receiver ${YARP_LIBRARIES})

add_executable(internal_monologue internal_monologue.cpp)
target_link_libraries(internal_monologue ${YARP_LIBRARIES})

add_executable(buffered_port buffered_port.cpp)
target_link_libraries(buffered_port ${YARP_LIBRARIES})

add_executable(portable_pair portable_pair.cpp)
target_link_libraries(portable_pair ${YARP_LIBRARIES})

add_executable(summer summer.cpp)
target_link_libraries(summer ${YARP_LIBRARIES})

add_executable(image_source image_source.cpp)
target_link_libraries(image_source ${YARP_LIBRARIES})

add_executable(database database.cpp)
target_link_libraries(database ${YARP_LIBRARIES})

add_executable(threads threads.cpp)
target_link_libraries(threads ${YARP_LIBRARIES})

add_executable(image_process image_process.cpp)
target_link_libraries(image_process ${YARP_LIBRARIES})

add_executable(image_process_module image_process_module.cpp)
target_link_libraries(image_process_module ${YARP_LIBRARIES})

add_executable(ratethread ratethread.cpp)
target_link_libraries(ratethread ${YARP_LIBRARIES})

add_executable(rateThreadTiming rateThreadTiming.cpp)
target_link_libraries(rateThreadTiming ${YARP_LIBRARIES})

add_executable(make_count make_count.cpp)
target_link_libraries(make_count ${YARP_LIBRARIES})

add_executable(view_count view_count.cpp)
target_link_libraries(view_count ${YARP_LIBRARIES})

add_executable(browse_bottle browse_bottle.cpp)
target_link_libraries(browse_bottle ${YARP_LIBRARIES})

add_executable(queue_manager queue_manager.cpp)
target_link_libraries(queue_manager ${YARP_LIBRARIES})

add_executable(playback playback.cpp)
target_link_libraries(playback ${YARP_LIBRARIES})

add_executable(listports listports.cpp)
target_link_libraries(listports ${YARP_LIBRARIES})

add_executable(rpc_server rpc_server.cpp)
target_link_libraries(rpc_server ${YARP_LIBRARIES})

add_executable(rpc_client rpc_client.cpp)
target_link_libraries(rpc_client ${YARP_LIBRARIES})
