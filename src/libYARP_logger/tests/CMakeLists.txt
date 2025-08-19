# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpCatchUtils)

add_executable(harness_logger)

target_sources(harness_logger PRIVATE
  LogForwarderTest.cpp
)

target_include_directories(harness_logger PRIVATE ${hmac_INCLUDE_DIRS})

target_link_libraries(harness_logger
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_logger
)

if(YARP_HAS_ACE)
  target_compile_definitions(harness_logger PRIVATE YARP_HAS_ACE)
  target_link_libraries(harness_logger PRIVATE ACE::ACE_INLINE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_libraries(harness_logger PRIVATE pthread)
endif()

set_property(TARGET harness_logger PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_logger)
