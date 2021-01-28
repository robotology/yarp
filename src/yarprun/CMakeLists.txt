# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(yarprun)
target_sources(yarprun PRIVATE yarprun.cpp)
target_link_libraries(yarprun PRIVATE YARP::YARP_os
                                      YARP::YARP_init
                                      YARP::YARP_run)

install(TARGETS yarprun COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
set_property(TARGET yarprun PROPERTY FOLDER "Command Line Tools")
