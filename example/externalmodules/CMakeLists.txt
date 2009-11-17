# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
FIND_PACKAGE(YARP REQUIRED)

BEGIN_DEVICE_LIBRARY(foobaz)
ADD_SUBDIRECTORY(baz)
ADD_SUBDIRECTORY(foo)
END_DEVICE_LIBRARY(foobaz)
