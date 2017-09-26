# Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


project(rtf_fixturemanager_yarpserver)

include_directories(SYSTEM ${RTF_INCLUDE_DIRS})

add_definitions(-DSHLIBPP_FILTER_API)

add_library(rtf_fixturemanager_yarpserver MODULE yarpserverfixture.cpp)
set_property(TARGET rtf_fixturemanager_yarpserver PROPERTY PREFIX "")
set_property(TARGET rtf_fixturemanager_yarpserver PROPERTY OUTPUT_NAME yarpserver)

target_link_libraries(rtf_fixturemanager_yarpserver PRIVATE RTF::RTF
                                                            YARP::YARP_OS
                                                            YARP::YARP_init
                                                            YARP::YARP_serversql)

yarp_install(TARGETS rtf_fixturemanager_yarpserver
             EXPORT YARP
             COMPONENT runtime
             LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/rtf)
