# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_serversql)

target_sources(harness_serversql PRIVATE ServerTest.cpp)

target_include_directories(harness_serversql PRIVATE ${hmac_INCLUDE_DIRS})

target_link_libraries(harness_serversql PRIVATE YARP_harness
                                                YARP::YARP_os
                                                YARP::YARP_serversql)
set_property(TARGET harness_serversql PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_serversql)
