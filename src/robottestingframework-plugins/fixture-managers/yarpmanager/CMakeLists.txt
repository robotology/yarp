# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(robottestingframework_fixturemanager_yarpmanager)

add_definitions(-DSHLIBPP_FILTER_API)

robottestingframework_add_plugin(robottestingframework_fixturemanager_yarpmanager OUTPUT_NAME yarpmanager
                                                                                  SOURCES YarpFixManager.cpp
                                                                                  HEADERS YarpFixManager.h)

target_link_libraries(robottestingframework_fixturemanager_yarpmanager PRIVATE RobotTestingFramework::RTF
                                                                               RobotTestingFramework::RTF_dll
                                                                               YARP::YARP_os
                                                                               YARP::YARP_init
                                                                               YARP::YARP_manager)

yarp_install(TARGETS robottestingframework_fixturemanager_yarpmanager
             EXPORT YARP_RobotTestingFramework_plugins
             COMPONENT YARP_RobotTestingFramework_plugins
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/robottestingframework)
