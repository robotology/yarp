# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# replace "${CMAKE_SOURCE_DIR}/.." with the path to your YARP binaries
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

PROJECT(attachable)

ADD_EXECUTABLE(attachable main.cpp)
