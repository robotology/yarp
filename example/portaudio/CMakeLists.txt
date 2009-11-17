# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Francesco Nori
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "$ENV{YARP_ROOT}" CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)
ADD_EXECUTABLE(sound_receiver sound_receiver.cpp)
ADD_EXECUTABLE(sound_sender sound_sender.cpp)
ADD_EXECUTABLE(sound_loopback sound_loopback.cpp)
