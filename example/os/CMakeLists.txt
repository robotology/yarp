# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(Examples)

# Be sure to set YARP_DIR environment variable if YARP is not installed
# somewhere findable.
find_package(YARP COMPONENTS os sig dev REQUIRED)

add_executable(bottle_add)
target_sources(bottle_add PRIVATE bottle_add.cpp)
target_link_libraries(bottle_add PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(simple_sender)
target_sources(simple_sender PRIVATE simple_sender.cpp)
target_link_libraries(simple_sender PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(simple_receiver)
target_sources(simple_receiver PRIVATE simple_receiver.cpp)
target_link_libraries(simple_receiver PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(internal_monologue)
target_sources(internal_monologue PRIVATE internal_monologue.cpp)
target_link_libraries(internal_monologue PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(buffered_port)
target_sources(buffered_port PRIVATE buffered_port.cpp)
target_link_libraries(buffered_port PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(portable_pair)
target_sources(portable_pair PRIVATE portable_pair.cpp)
target_link_libraries(portable_pair PRIVATE YARP::YARP_os YARP::YARP_init YARP::YARP_sig)

add_executable(summer)
target_sources(summer PRIVATE summer.cpp)
target_link_libraries(summer PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(image_source)
target_sources(image_source PRIVATE image_source.cpp)
target_link_libraries(image_source PRIVATE YARP::YARP_os YARP::YARP_init YARP::YARP_sig)

add_executable(database)
target_sources(database PRIVATE database.cpp)
target_link_libraries(database PRIVATE YARP::YARP_os YARP::YARP_init YARP::YARP_dev)

add_executable(threads)
target_sources(threads PRIVATE threads.cpp)
target_link_libraries(threads PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(image_process)
target_sources(image_process PRIVATE image_process.cpp)
target_link_libraries(image_process PRIVATE YARP::YARP_os YARP::YARP_init YARP::YARP_sig)

add_executable(image_process_module)
target_sources(image_process_module PRIVATE image_process_module.cpp)
target_link_libraries(image_process_module PRIVATE YARP::YARP_os YARP::YARP_init YARP::YARP_sig)

add_executable(ratethread)
target_sources(ratethread PRIVATE ratethread.cpp)
target_link_libraries(ratethread PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(rateThreadTiming)
target_sources(rateThreadTiming PRIVATE rateThreadTiming.cpp)
target_link_libraries(rateThreadTiming PRIVATE YARP::YARP_os YARP::YARP_init YARP::YARP_sig)

add_executable(make_count)
target_sources(make_count PRIVATE make_count.cpp)
target_link_libraries(make_count PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(view_count)
target_sources(view_count PRIVATE view_count.cpp)
target_link_libraries(view_count PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(browse_bottle)
target_sources(browse_bottle PRIVATE browse_bottle.cpp)
target_link_libraries(browse_bottle PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(queue_manager)
target_sources(queue_manager PRIVATE queue_manager.cpp)
target_link_libraries(queue_manager PRIVATE YARP::YARP_os YARP::YARP_init YARP::YARP_dev)

add_executable(playback)
target_sources(playback PRIVATE playback.cpp)
target_link_libraries(playback PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(listports)
target_sources(listports PRIVATE listports.cpp)
target_link_libraries(listports PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(rpc_server)
target_sources(rpc_server PRIVATE rpc_server.cpp)
target_link_libraries(rpc_server PRIVATE YARP::YARP_os YARP::YARP_init)

add_executable(rpc_client)
target_sources(rpc_client PRIVATE rpc_client.cpp)
target_link_libraries(rpc_client PRIVATE YARP::YARP_os YARP::YARP_init)
