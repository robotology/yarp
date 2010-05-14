# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Alessandro Scalzo
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
ADD_EXECUTABLE(yarprun yarprun.cpp)
TARGET_LINK_LIBRARIES(yarprun YARP_init)

INSTALL(TARGETS yarprun COMPONENT utilities DESTINATION bin)
