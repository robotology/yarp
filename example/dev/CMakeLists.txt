# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giacomo Spigler
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

FIND_PACKAGE(YARP REQUIRED)
LINK_LIBRARIES(${YARP_LIBRARIES})
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(grabber_client grabber_client.cpp)
ADD_EXECUTABLE(grabber_client_alternate grabber_client_alternate.cpp)
ADD_EXECUTABLE(fake_grabber fake_grabber.cpp FakeFrameGrabber.h)
ADD_EXECUTABLE(fake_grabber_net fake_grabber_net.cpp FakeFrameGrabber.h)
ADD_EXECUTABLE(fake_grabber_net2 fake_grabber_net2.cpp FakeFrameGrabber2.h)
ADD_EXECUTABLE(file_grabber file_grabber.cpp FileFrameGrabber.h)
ADD_EXECUTABLE(file_grabber_net file_grabber_net.cpp FileFrameGrabber.h)
ADD_EXECUTABLE(audio_test audio_test.cpp)
ADD_EXECUTABLE(double_server double_server.cpp)
ADD_EXECUTABLE(fake_motor fake_motor.cpp)
ADD_EXECUTABLE(simple_motor_client simple_motor_client.cpp)
ADD_EXECUTABLE(motortest motortest.cpp)
