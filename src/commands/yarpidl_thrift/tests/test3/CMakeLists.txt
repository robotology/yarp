# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include (Catch)

function(yarp_catch_discover_tests _target)
  # Workaround to force catch_discover_tests to run tests under valgrind
  set_property(TARGET ${_target} PROPERTY CROSSCOMPILING_EMULATOR "${YARP_TEST_LAUNCHER}")
  catch_discover_tests(
    ${_target}
    EXTRA_ARGS "-s" "--colour-mode default"
    PROPERTIES
      TIMEOUT ${YARP_TEST_TIMEOUT}
      SKIP_RETURN_CODE 254
    )
endfunction()

include(YarpChooseIDL)
yarp_choose_idl(TEST3_THRIFT testdata.thrift)

add_executable(harness_thrift_test3)

target_sources(harness_thrift_test3
  PRIVATE
    main.cpp
    ${TEST3_THRIFT_GEN_FILES}
)
target_link_libraries(harness_thrift_test3
  PRIVATE
    YARP_harness
    YARP::YARP_os

)
target_include_directories(harness_thrift_test3
 PUBLIC ${TEST3_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS}
)

set_property(TARGET harness_thrift_test3 PROPERTY FOLDER "Test")
yarp_catch_discover_tests(harness_thrift_test3)
