# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Alessandro Scalzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 

GET_PROPERTY(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
SET_TARGET_PROPERTIES( ${name} PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
INCLUDE_DIRECTORIES(${YARP_TREE_INCLUDE_DIRS})
include_directories(${ACE_INCLUDE_DIRS})
ADD_EXECUTABLE(yarprun yarprun.cpp)
TARGET_LINK_LIBRARIES(yarprun YARP_OS YARP_init)

INSTALL(TARGETS yarprun COMPONENT utilities DESTINATION bin)
