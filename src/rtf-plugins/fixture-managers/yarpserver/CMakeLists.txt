# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(rtf_fixturemanager_yarpserver)

include_directories(SYSTEM ${RTF_INCLUDE_DIRS})

add_definitions(-DSHLIBPP_FILTER_API)

rtf_add_plugin(rtf_fixturemanager_yarpserver OUTPUT_NAME yarpserver
                                             SOURCES     yarpserverfixture.cpp)

target_link_libraries(rtf_fixturemanager_yarpserver PRIVATE RTF::RTF
                                                            RTF::RTF_dll
                                                            YARP::YARP_OS
                                                            YARP::YARP_init
                                                            YARP::YARP_serversql)

yarp_install(TARGETS rtf_fixturemanager_yarpserver
             EXPORT YARP_RTF_plugins
             COMPONENT YARP_RTF_plugins
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rtf)
