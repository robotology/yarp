# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Checks the value returned by the generated client methods when the
# communication fails (yarp::dev::ReturnValue must report
# return_value_error_nws_nwc_communication_error).

include (Catch)
include(YarpCatchUtils)
include(YarpChooseIDL)

yarp_choose_idl(TEST4_THRIFT testreturnvalue.thrift)

add_executable(harness_thrift_test4)

target_sources(harness_thrift_test4
  PRIVATE
    main.cpp
    ${TEST4_THRIFT_GEN_FILES}
)

target_link_libraries(harness_thrift_test4
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_dev
)

target_include_directories(harness_thrift_test4
 PUBLIC ${TEST4_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS}
)

set_property(TARGET harness_thrift_test4 PROPERTY FOLDER "Test")
yarp_catch_discover_tests(harness_thrift_test4)
