# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)

find_package(YARP COMPONENTS os sig dev REQUIRED)

add_executable(grabber_client)
target_sources(grabber_client PRIVATE grabber_client.cpp)
target_link_libraries(grabber_client PRIVATE ${YARP_LIBRARIES})

add_executable(grabber_client_alternate)
target_sources(grabber_client_alternate PRIVATE grabber_client_alternate.cpp)
target_link_libraries(grabber_client_alternate PRIVATE ${YARP_LIBRARIES})

add_executable(fake_grabber)
target_sources(fake_grabber PRIVATE fake_grabber.cpp FakeFrameGrabber.h)
target_link_libraries(fake_grabber PRIVATE ${YARP_LIBRARIES})

add_executable(fake_grabber_net)
target_sources(fake_grabber_net PRIVATE fake_grabber_net.cpp FakeFrameGrabber.h)
target_link_libraries(fake_grabber_net PRIVATE ${YARP_LIBRARIES})

# add_executable(fake_grabber_net2)
# target_sources(fake_grabber_net2 PRIVATE fake_grabber_net2.cpp FakeFrameGrabber2.h)
# target_link_libraries(fake_grabber_net2 PRIVATE ${YARP_LIBRARIES})

add_executable(file_grabber)
target_sources(file_grabber PRIVATE file_grabber.cpp FileFrameGrabber.h)
target_link_libraries(file_grabber PRIVATE ${YARP_LIBRARIES})

add_executable(file_grabber_net)
target_sources(file_grabber_net PRIVATE file_grabber_net.cpp FileFrameGrabber.h)
target_link_libraries(file_grabber_net PRIVATE ${YARP_LIBRARIES})

# add_executable(audio_test)
# target_sources(audio_test PRIVATE audio_test.cpp)
# target_link_libraries(audio_test PRIVATE ${YARP_LIBRARIES})

add_executable(double_server)
target_sources(double_server PRIVATE double_server.cpp)
target_link_libraries(double_server PRIVATE ${YARP_LIBRARIES})

# add_executable(fake_motor)
# target_sources(fake_motor PRIVATE fake_motor.cpp)
# target_link_libraries(fake_motor PRIVATE ${YARP_LIBRARIES})

# add_executable(simple_motor_client)
# target_sources(simple_motor_client PRIVATE simple_motor_client.cpp)
# target_link_libraries(simple_motor_client PRIVATE ${YARP_LIBRARIES})

add_executable(motortest)
target_sources(motortest PRIVATE motortest.cpp)
target_link_libraries(motortest PRIVATE ${YARP_LIBRARIES})

add_subdirectory(RGBD)
