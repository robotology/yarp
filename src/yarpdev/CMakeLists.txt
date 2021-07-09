# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_executable(yarpdev)
target_sources(yarpdev PRIVATE yarpdev.cpp)
target_link_libraries(yarpdev PRIVATE YARP::YARP_init
                                      YARP::YARP_dev)

install(TARGETS yarpdev
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})

set_property(TARGET yarpdev PROPERTY FOLDER "Command Line Tools")
