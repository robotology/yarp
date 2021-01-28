# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(yarp)
target_sources(yarp PRIVATE yarp.cpp)
target_link_libraries(yarp PRIVATE YARP::YARP_os
                                   YARP::YARP_init
                                   YARP::YARP_serversql
                                   YARP::YARP_run
                                   YARP::YARP_companion)

install(TARGETS yarp
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarp PROPERTY FOLDER "Command Line Tools")
