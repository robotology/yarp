# Copyright: (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


project(rtf_fixturemanager_yarpmanager)

get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)

include_directories(SYSTEM ${RTF_INCLUDE_DIRS})
include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${YARP_manager_INCLUDE_DIRS})

add_library(rtf_fixturemanager_yarpmanager MODULE YarpFixManager.h
                                                  YarpFixManager.cpp)
set_property(TARGET rtf_fixturemanager_yarpmanager PROPERTY PREFIX "")
set_property(TARGET rtf_fixturemanager_yarpmanager PROPERTY OUTPUT_NAME yarpmanager)

target_link_libraries(rtf_fixturemanager_yarpmanager PRIVATE RTF::RTF
                                                             YARP::YARP_OS
                                                             YARP::YARP_manager)

yarp_install(TARGETS rtf_fixturemanager_yarpmanager
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rtf)
