# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(yarpdev)
target_sources(yarpdev PRIVATE yarpdev.cpp)
target_link_libraries(yarpdev PRIVATE YARP::YARP_init
                                      YARP::YARP_dev)

install(TARGETS yarpdev
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarpdev PROPERTY FOLDER "Command Line Tools")
