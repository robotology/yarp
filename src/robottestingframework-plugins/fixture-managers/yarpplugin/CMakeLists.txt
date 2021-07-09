# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

project(robottestingframework_fixturemanager_yarpplugin)

add_definitions(-DSHLIBPP_FILTER_API)

robottestingframework_add_plugin(robottestingframework_fixturemanager_yarpplugin OUTPUT_NAME yarpplugin
                                                                                 SOURCES YarpPluginFixture.cpp
                                                                                 HEADERS YarpPluginFixture.h)

target_link_libraries(robottestingframework_fixturemanager_yarpplugin PRIVATE RobotTestingFramework::RTF
                                                                              RobotTestingFramework::RTF_dll
                                                                              YARP::YARP_os
                                                                              YARP::YARP_init
                                                                              YARP::YARP_dev)

yarp_install(TARGETS robottestingframework_fixturemanager_yarpplugin
             EXPORT YARP_RobotTestingFramework_plugins
             COMPONENT YARP_RobotTestingFramework_plugins
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/robottestingframework)
