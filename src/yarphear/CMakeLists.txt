# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(yarphear)
target_sources(yarphear PRIVATE yarphear.cpp)
target_link_libraries(yarphear PRIVATE YARP::YARP_os
                                       YARP::YARP_sig
                                       YARP::YARP_dev
                                       YARP::YARP_init)

install(TARGETS yarphear
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarphear PROPERTY FOLDER "Command Line Tools")
