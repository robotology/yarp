# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

ADD_EXECUTABLE(framerate main.cpp)
TARGET_LINK_LIBRARIES(framerate ${YARP_LIBRARIES})
INSTALL_TARGETS(/bin framerate)

