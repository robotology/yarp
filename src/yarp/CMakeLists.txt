# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
ADD_EXECUTABLE(yarp yarp.cpp)
TARGET_LINK_LIBRARIES(yarp YARP_init)
ADD_EXECUTABLE(yarpserver yarpserver.cpp)
TARGET_LINK_LIBRARIES(yarpserver YARP_init)

INSTALL(TARGETS yarp yarpserver COMPONENT utilities DESTINATION bin)
