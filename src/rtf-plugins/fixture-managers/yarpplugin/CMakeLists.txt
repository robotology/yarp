# Copyright: (C) 2016, 2017 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

project(rtf_fixturemanager_yarpplugin)

get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_rtf PROPERTY INCLUDE_DIRS)

include_directories(SYSTEM ${RTF_INCLUDE_DIRS})
include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${YARP_dev_INCLUDE_DIRS}
                    ${YARP_rtf_INCLUDE_DIRS})

add_library(rtf_fixturemanager_yarpplugin MODULE YarpPluginFixture.h
                                                 YarpPluginFixture.cpp)
set_property(TARGET rtf_fixturemanager_yarpplugin PROPERTY PREFIX "")
set_property(TARGET rtf_fixturemanager_yarpplugin PROPERTY OUTPUT_NAME yarpplugin)

target_link_libraries(rtf_fixturemanager_yarpplugin RTF::RTF
                                                    YARP::YARP_OS
                                                    YARP::YARP_dev
                                                    YARP::YARP_rtf)

yarp_install(TARGETS rtf_fixturemanager_yarpplugin
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rtf)
