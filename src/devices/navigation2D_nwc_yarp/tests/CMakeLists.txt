# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

#########################################################################
# Wrapper for the catch_discover_tests that also enables colors, and sets
# the TIMEOUT and SKIP_RETURN_CODE test properties.
include(Catch)
function(yarp_catch_discover_tests _target)
  # Workaround to force catch_discover_tests to run tests under valgrind
  set_property(TARGET ${_target} PROPERTY CROSSCOMPILING_EMULATOR "${YARP_TEST_LAUNCHER}")
  catch_discover_tests(
    ${_target}
    EXTRA_ARGS "-s" "--use-colour" "yes"
    PROPERTIES
      TIMEOUT ${YARP_TEST_TIMEOUT}
      SKIP_RETURN_CODE 254
    )
endfunction()
#########################################################################

find_package(YARP::YARP_dev_tests)

add_executable(harness_dev_Navigation2Dnwc)

target_sources(harness_dev_Navigation2Dnwc
  PRIVATE
    Navigation2DnwcTest.cpp
)

target_link_libraries(harness_dev_Navigation2Dnwc
  PRIVATE
    YARP_harness
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_dev
    YARP::YARP_dev_tests
)

if(TARGET YARP::YARP_math)
  target_link_libraries(harness_dev_Navigation2Dnwc
  PRIVATE
    YARP::YARP_math
  )
else()
  set(_disabled_files
    Navigation2DnwcTest.cpp
  )
  set_source_files_properties(${_disabled_files} PROPERTIES HEADER_FILE_ONLY ON)
endif()

set_property(TARGET harness_dev_Navigation2Dnwc PROPERTY FOLDER "Test")

yarp_catch_discover_tests(harness_dev_Navigation2Dnwc)
