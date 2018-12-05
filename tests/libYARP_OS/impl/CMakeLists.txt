# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(harness_os_impl BottleImplTest.cpp
                               BufferedConnectionWriterTest.cpp
                               DgramTwoWayStreamTest.cpp
                               NameConfigTest.cpp
                               NameServerTest.cpp
                               PortCommandTest.cpp
                               PortCoreTest.cpp
                               ProtocolTest.cpp
                               StreamConnectionReaderTest.cpp)

target_include_directories(harness_os_impl PRIVATE ${ACE_INCLUDE_DIRS})

target_link_libraries(harness_os_impl PRIVATE YARP_harness
                                              YARP::YARP_OS
                                              YARP::YARP_sig
                                              ${ACE_LIBRARIES})

set_property(TARGET harness_os_impl PROPERTY FOLDER "Test")

yarp_parse_and_add_catch_tests(harness_os_impl)
