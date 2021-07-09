# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

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
