# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

# make sure YARP_DIR environment variable is set
FIND_PACKAGE(YARP REQUIRED)

PROJECT(dev_check)

ADD_EXECUTABLE(dev_check main.cpp)
