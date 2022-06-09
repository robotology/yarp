# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_executable(harness_devices)
target_sources(harness_devices
  PRIVATE
    harness_devices.cpp
)
target_link_libraries(harness_devices
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
    YARP::YARP_dev
)

target_compile_definitions(harness_devices
  PRIVATE
    CMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
)

set_property(TARGET harness_devices PROPERTY FOLDER "Test")

#####################################################################
function(yarp_test_device _test_name _ini_filename)

  set (_ini_file ${CMAKE_CURRENT_SOURCE_DIR}/tests/${_ini_filename})
  get_filename_component(_ini_file ${_ini_file} ABSOLUTE)

  #The following command, used later on, evaluates to the full path of the harness_devices file
  #message ($<TARGET_FILE:harness_devices>)

  # need to cache examples for reference (2022 change - removed doc stuff)
  set(_out_dox "${CMAKE_SOURCE_DIR}/doc/device_invocation/${_test_name}.dox")

  # set(_cmd ${YARP_TEST_LAUNCHER} $<TARGET_FILE:harness_devices> --file "${_ini_file}" --doc "${_out_dox}" --verbose)
  set(_cmd ${YARP_TEST_LAUNCHER} $<TARGET_FILE:harness_devices> --file "${_ini_file}" --verbose)

  # Add normal test
  add_test(
  NAME "device::${_test_name}"
  COMMAND ${_cmd}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  set_property(TEST "device::${_test_name}" PROPERTY SKIP_RETURN_CODE 254)
  set_property(TEST "device::${_test_name}" PROPERTY TIMEOUT ${YARP_TEST_TIMEOUT})

endfunction()
