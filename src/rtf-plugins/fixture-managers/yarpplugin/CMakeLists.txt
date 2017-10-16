# Copyright: (C) 2016, 2017 Istituto Italiano di Tecnologia (IIT)
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


project(rtf_fixturemanager_yarpplugin)

include_directories(SYSTEM ${RTF_INCLUDE_DIRS})

add_definitions(-DSHLIBPP_FILTER_API)

rtf_add_plugin(rtf_fixturemanager_yarpplugin OUTPUT_NAME yarpplugin
                                             SOURCES     YarpPluginFixture.cpp
                                             HEADERS     YarpPluginFixture.h)

target_link_libraries(rtf_fixturemanager_yarpplugin PRIVATE RTF::RTF
                                                            RTF::RTF_dll
                                                            YARP::YARP_OS
                                                            YARP::YARP_init
                                                            YARP::YARP_dev)

yarp_install(TARGETS rtf_fixturemanager_yarpplugin
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rtf)
