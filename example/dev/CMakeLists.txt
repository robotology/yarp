# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

foreach(example_name
  attachable
  audio_test
  double_server
  fake_grabber
  fake_motor
  file_grabber
  grabber_client
  grabber_crop
  motorcontrol
  motortest
  RGBD
)

  if(YARP_COMPILE_EXAMPLES)
    add_subdirectory(${example_name})
  endif()

  if(YARP_ENABLE_EXAMPLES_AS_TESTS)
    add_test(NAME examples::dev::${example_name}
             COMMAND ${CMAKE_CTEST_COMMAND} -VV
                                            --build-and-test "${CMAKE_CURRENT_SOURCE_DIR}/${example_name}"
                                                             "${CMAKE_CURRENT_BINARY_DIR}/tests/${example_name}"
                                            ${build_generator}
                                            --build-project  ${example_name}
                                            --build-options ${build_options}
             WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    set_tests_properties(examples::dev::${example_name} PROPERTIES LABELS "examples::dev")
    list(APPEND EXAMPLES_BUILD_DIRS "${CMAKE_CURRENT_BINARY_DIR}/tests/${example_name}")
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${CMAKE_CURRENT_BINARY_DIR}/tests/${example_name}")

  endif()

endforeach()
