# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpCatchUtils)

add_executable(harness_serversql1)

target_sources(harness_serversql1
  PRIVATE
    ServerTest1.cpp
)

target_include_directories(harness_serversql1
  PRIVATE
    ${hmac_INCLUDE_DIRS}
)

target_link_libraries(harness_serversql1
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_serversql
)
set_property(TARGET harness_serversql1 PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_serversql1)

###########################################

add_executable(harness_serversql2)

target_sources(harness_serversql2
  PRIVATE
    ServerTest2.cpp
)

target_include_directories(harness_serversql2
  PRIVATE
    ${hmac_INCLUDE_DIRS}
)

target_link_libraries(harness_serversql2
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_serversql
)
set_property(TARGET harness_serversql2 PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_serversql2)
