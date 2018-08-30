# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

macro(add_ruby_unit_test rubyscript)
  if(RUBY_EXECUTABLE)
    add_test(NAME ${rubyscript} COMMAND ${RUBY_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${rubyscript})
    set_tests_properties(${rubyscript} PROPERTIES ENVIRONMENT "RUBYLIB=$<TARGET_FILE_DIR:${SWIG_MODULE_yarp_ruby_REAL_NAME}>")
  endif()
endmacro()

file(GLOB yarp_ruby_test_files "${CMAKE_CURRENT_SOURCE_DIR}/*.rb")

add_ruby_unit_test(test_buffered_port.rb)
add_ruby_unit_test(test_image.rb)
add_ruby_unit_test(test_vector.rb)
