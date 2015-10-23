# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Alessandro Scalzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)

include_directories(${YARP_OS_INCLUDE_DIRS})
include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

add_executable(yarprun yarprun.cpp)
target_link_libraries(yarprun YARP_OS YARP_init)

install(TARGETS yarprun COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
