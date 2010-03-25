# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# Make sure that YARP_DIR is set in your environment to wherever you 
# compiled YARP.

PROJECT(example)

FIND_PACKAGE(YARP)

ADD_EXECUTABLE(example main.cpp)
