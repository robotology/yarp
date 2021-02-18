# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(harness_run)

target_sources(harness_run PRIVATE RunTest.cpp)

target_link_libraries(harness_run PRIVATE YARP_harness
                                          YARP::YARP_os
                                          YARP::YARP_run)

set_property(TARGET harness_run PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_run)

get_property(_tests SOURCE RunTest.cpp PROPERTY ParseAndAddCatchTests_TESTS)
foreach(_test IN LISTS _tests)
  set_property(TEST ${_test} PROPERTY TIMEOUT 15)
endforeach()


# Test module started by the unit tests
add_executable(testModule)
target_sources(testModule PRIVATE module.cpp)
target_link_libraries(testModule PRIVATE YARP::YARP_init
                                         YARP::YARP_os)
set_property(TARGET testModule PROPERTY FOLDER "Test")
