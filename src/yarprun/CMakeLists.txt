# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Alessandro Scalzo
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
INCLUDE_DIRECTORIES(${YARP_OS_INC})
LINK_LIBRARIES(YARP_OS)

########################################################################
##
## yarprun -- command line
##

ADD_EXECUTABLE(yarprun yarprun.cpp)

TARGET_LINK_LIBRARIES(yarprun YARP_OS)
IF(MSVC)
    SET_TARGET_PROPERTIES(yarprun PROPERTIES COMPILE_FLAGS /wd4996)
ENDIF(MSVC)


INSTALL_TARGETS(/bin yarprun)
