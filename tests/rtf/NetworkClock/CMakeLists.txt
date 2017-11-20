# Copyright: (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Silvio Traversaro <silvio.traversaro@iit.it>
# Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

add_definitions(-DSHLIBPP_FILTER_API)
rtf_add_plugin(NetworkClockTest SOURCES NetworkClockTest.cpp)
# add_library(NetworkClockTest MODULE NetworkClockTest.cpp)
target_link_libraries(NetworkClockTest YARP_init YARP_OS YARP_rtf ${RTF_LIBRARIES})
rtf_add_suite(NAME NetworkClockTest
              RTF_FIXTURE rtf_fixturemanager_yarpserver --silent
              RTF_FIXTURE rtf_fixturemanager_yarpmanager --fixture ${CMAKE_CURRENT_SOURCE_DIR}/fixture-network-clock.xml
              RTF_TEST NetworkClockTest TYPE dll
              VERBOSE)
set_tests_properties(NetworkClockTest PROPERTIES TIMEOUT 30.0)
