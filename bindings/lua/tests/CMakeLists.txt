# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

find_program(LUA_EXECUTABLE
             NAMES lua${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
                   lua)
mark_as_advanced(LUA_EXECUTABLE)

function(add_lua_unit_test luascript)
  if(LUA_EXECUTABLE)
    string(REGEX REPLACE "^test_" "bindings::lua::" name "${luascript}")
    string(REGEX REPLACE "\\.lua$" "" name "${name}")
    add_test(NAME "${name}" COMMAND ${LUA_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${luascript})
    set_tests_properties("${name}" PROPERTIES ENVIRONMENT "LUA_CPATH_${LUA_VERSION_MAJOR}_${LUA_VERSION_MINOR}=\;\;\;$<TARGET_FILE:${SWIG_MODULE_yarp_lua_REAL_NAME}>")
    set_tests_properties("${name}" PROPERTIES TIMEOUT 60)
  endif()
endfunction()

add_lua_unit_test(test_bottle_and_property.lua)
add_lua_unit_test(test_image.lua)
add_lua_unit_test(test_port.lua)
add_lua_unit_test(test_resource_finder.lua)
add_lua_unit_test(test_time_delay.lua)
add_lua_unit_test(test_vector.lua)
add_lua_unit_test(test_vocab.lua)
