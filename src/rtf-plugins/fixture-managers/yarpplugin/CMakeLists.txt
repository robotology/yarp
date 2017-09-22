# Copyright: (C) 2016, 2017 Istituto Italiano di Tecnologia (IIT)
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


project(rtf_fixturemanager_yarpplugin)

include_directories(SYSTEM ${RTF_INCLUDE_DIRS})

add_definitions(-DSHLIBPP_FILTER_API)

add_library(rtf_fixturemanager_yarpplugin MODULE YarpPluginFixture.h
                                                 YarpPluginFixture.cpp)
set_property(TARGET rtf_fixturemanager_yarpplugin PROPERTY PREFIX "")
set_property(TARGET rtf_fixturemanager_yarpplugin PROPERTY OUTPUT_NAME yarpplugin)

target_link_libraries(rtf_fixturemanager_yarpplugin RTF::RTF
                                                    YARP::YARP_OS
                                                    YARP::YARP_init
                                                    YARP::YARP_dev)

yarp_install(TARGETS rtf_fixturemanager_yarpplugin
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rtf)
