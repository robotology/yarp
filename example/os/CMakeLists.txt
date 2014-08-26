# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Giacomo Spigler
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

PROJECT (Examples)

# Be sure to set YARP_DIR environment variable if YARP is not installed
# somewhere findable.
FIND_PACKAGE(YARP REQUIRED)
LINK_LIBRARIES(${YARP_LIBRARIES})
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(bottle_add bottle_add.cpp)
ADD_EXECUTABLE(simple_sender simple_sender.cpp)
ADD_EXECUTABLE(simple_receiver simple_receiver.cpp)
ADD_EXECUTABLE(internal_monologue internal_monologue.cpp)
ADD_EXECUTABLE(buffered_port buffered_port.cpp)
ADD_EXECUTABLE(portable_pair portable_pair.cpp)
ADD_EXECUTABLE(summer summer.cpp)
ADD_EXECUTABLE(image_source image_source.cpp)
ADD_EXECUTABLE(database database.cpp)
ADD_EXECUTABLE(threads threads.cpp)
ADD_EXECUTABLE(image_process image_process.cpp)
ADD_EXECUTABLE(image_process_module image_process_module.cpp)
ADD_EXECUTABLE(ratethread ratethread.cpp)
ADD_EXECUTABLE(rateThreadTiming rateThreadTiming.cpp)
ADD_EXECUTABLE(make_count make_count.cpp)
ADD_EXECUTABLE(view_count view_count.cpp)
ADD_EXECUTABLE(browse_bottle browse_bottle.cpp)
ADD_EXECUTABLE(queue_manager queue_manager.cpp)
ADD_EXECUTABLE(playback playback.cpp)
ADD_EXECUTABLE(listports listports.cpp)
ADD_EXECUTABLE(rpc_server rpc_server.cpp)
ADD_EXECUTABLE(rpc_client rpc_client.cpp)

