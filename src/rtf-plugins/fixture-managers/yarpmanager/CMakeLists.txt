# Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


project(rtf_fixturemanager_yarpmanager)

include_directories(SYSTEM ${RTF_INCLUDE_DIRS})

add_definitions(-DSHLIBPP_FILTER_API)

add_library(rtf_fixturemanager_yarpmanager MODULE YarpFixManager.h
                                                  YarpFixManager.cpp)
set_property(TARGET rtf_fixturemanager_yarpmanager PROPERTY PREFIX "")
set_property(TARGET rtf_fixturemanager_yarpmanager PROPERTY OUTPUT_NAME yarpmanager)

target_link_libraries(rtf_fixturemanager_yarpmanager PRIVATE RTF::RTF
                                                             YARP::YARP_OS
                                                             YARP::YARP_init
                                                             YARP::YARP_manager)

yarp_install(TARGETS rtf_fixturemanager_yarpmanager
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rtf)
