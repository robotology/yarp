# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

macro(add_ruby_unit_test rubyscript)
  if(RUBY_EXECUTABLE)
    string(REGEX REPLACE "^test_" "bindings::ruby::" name "${rubyscript}")
    string(REGEX REPLACE "\\.rb$" "" name "${name}")
    add_test(NAME "${name}" COMMAND ${RUBY_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${rubyscript})
    set_tests_properties("${name}" PROPERTIES ENVIRONMENT "RUBYLIB=$<TARGET_FILE_DIR:${SWIG_MODULE_yarp_ruby_REAL_NAME}>")
    set_tests_properties("${name}" PROPERTIES TIMEOUT 60)
  endif()
endmacro()

add_ruby_unit_test(test_buffered_port.rb)
add_ruby_unit_test(test_image.rb)
add_ruby_unit_test(test_vector.rb)
