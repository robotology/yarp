# Copyright: (C) 2017 RobotCub Consortium
# Authors: Andrea Ruzzenenti
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
include_directories(${YARP_TREE_INCLUDE_DIRS})
add_executable(yarpframecontroller yarpframecontroller.cpp ParamParser.h)
target_link_libraries(yarpframecontroller YARP_init YARP_math YARP_dev YARP_OS YARP_math YARP_sig)

install(TARGETS yarpframecontroller
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarpframecontroller PROPERTY FOLDER "Command Line Tools")
