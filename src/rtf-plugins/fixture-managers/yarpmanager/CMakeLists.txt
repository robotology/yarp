# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(rtf_fixturemanager_yarpmanager)

include_directories(SYSTEM ${RTF_INCLUDE_DIRS})

add_definitions(-DSHLIBPP_FILTER_API)

rtf_add_plugin(rtf_fixturemanager_yarpmanager OUTPUT_NAME yarpmanager
                                              SOURCES     YarpFixManager.cpp
                                              HEADERS     YarpFixManager.h)

target_link_libraries(rtf_fixturemanager_yarpmanager PRIVATE RTF::RTF
                                                             RTF::RTF_dll
                                                             YARP::YARP_OS
                                                             YARP::YARP_init
                                                             YARP::YARP_manager)

yarp_install(TARGETS rtf_fixturemanager_yarpmanager
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rtf)
