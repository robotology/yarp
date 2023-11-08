# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpCatchUtils)

add_executable(harness_test1)

target_sources(harness_test1 PRIVATE
  harness_test1.cpp
)

target_link_libraries(harness_test1
  PRIVATE
    YARP_harness
    YARP::YARP_os
)

set_property(TARGET harness_test1 PROPERTY FOLDER "Test")
yarp_catch_discover_tests(harness_test1)

####

add_executable(harness_test2)

target_sources(harness_test2 PRIVATE
  harness_test2.cpp
)

target_link_libraries(harness_test2
  PRIVATE
    YARP_harness
    YARP::YARP_os
)

set_property(TARGET harness_test2 PROPERTY FOLDER "Test")
yarp_catch_discover_tests(harness_test2)

####

add_executable(harness_test3)

target_sources(harness_test3 PRIVATE
  harness_test3.cpp
)

target_link_libraries(harness_test3
  PRIVATE
    YARP_harness_no_network
)

set_property(TARGET harness_test3 PROPERTY FOLDER "Test")
yarp_catch_discover_tests(harness_test3)
