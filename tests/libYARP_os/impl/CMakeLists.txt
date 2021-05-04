# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

add_executable(harness_os_impl)

target_sources(harness_os_impl PRIVATE BottleImplTest.cpp
                                       BufferedConnectionWriterTest.cpp
                                       DgramTwoWayStreamTest.cpp
                                       NameConfigTest.cpp
                                       NameServerTest.cpp
                                       PortCommandTest.cpp
                                       PortCoreTest.cpp
                                       ProtocolTest.cpp
                                       StreamConnectionReaderTest.cpp)

target_link_libraries(harness_os_impl PRIVATE YARP_harness
                                              YARP::YARP_os
                                              YARP::YARP_sig)

if(YARP_HAS_ACE)
  target_compile_definitions(harness_os_impl PRIVATE YARP_HAS_ACE)
  target_link_libraries(harness_os_impl PRIVATE ACE::ACE)
endif()

set_property(TARGET harness_os_impl PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_os_impl)
