# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

set(yarpserver_SRCS src/main.cpp)

add_executable(yarpserver)
target_sources(yarpserver PRIVATE ${yarpserver_SRCS})
target_link_libraries(yarpserver PRIVATE YARP::YARP_serversql)

install(TARGETS yarpserver
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})
set_property(TARGET yarpserver PROPERTY FOLDER "Command Line Tools")
