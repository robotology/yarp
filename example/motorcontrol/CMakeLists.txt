# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale, Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

PROJECT (Examples)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)
  ADD_EXECUTABLE(motorcontrol main.cpp)
ENDIF(GSL_FOUND)

