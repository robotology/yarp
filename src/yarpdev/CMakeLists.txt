# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.


GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
ADD_EXECUTABLE(yarpdev yarpdev.cpp)
TARGET_LINK_LIBRARIES(yarpdev YARP_dev)

INSTALL_TARGETS(/bin yarpdev)
