# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_os_impl)

target_sources(harness_os_impl
  PRIVATE
    BottleImplTest.cpp
    BufferedConnectionWriterTest.cpp
    DgramTwoWayStreamTest.cpp
    NameConfigTest.cpp
    NameServerTest.cpp
    PortCommandTest.cpp
    PortCoreTest.cpp
    ProtocolTest.cpp
    StreamConnectionReaderTest.cpp
)

target_link_libraries(harness_os_impl
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
)

if(YARP_HAS_ACE)
  target_compile_definitions(harness_os_impl PRIVATE YARP_HAS_ACE)
  target_link_libraries(harness_os_impl PRIVATE ACE::ACE)
endif()

set_property(TARGET harness_os_impl PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_os_impl)
