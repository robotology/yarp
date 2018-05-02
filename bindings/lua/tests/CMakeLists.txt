# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# test_string.lua uses the yarpserver, so for now it is not include in the CTest infrastructure

macro(add_lua_unit_test luascript)
  # Find the lua interpreter (ideally, this should be handled in FindLua or in a separate
  # FindLuaInterp like for python)
  # set_tests_properties(... ENVIRONMENT ) is only supported CMake 3.3
  if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.3)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
  endif()
  if(NOT ${CMAKE_VERSION} VERSION_LESS 3.3)
    find_program(LUA_EXECUTABLE
                 NAMES lua
                       lua5.3
                       lua5.2
                       lua5.1)
    if(LUA_EXECUTABLE)
      add_test(NAME ${luascript} COMMAND ${LUA_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${luascript})
      set_tests_properties(${luascript} PROPERTIES ENVIRONMENT "LUA_CPATH=\;\;\;$<TARGET_FILE:${SWIG_MODULE_yarp_lua_REAL_NAME}>")
    endif()
  endif()
endmacro()

add_lua_unit_test(test_bottle_and_property.lua)
add_lua_unit_test(test_resource_finder.lua)
add_lua_unit_test(test_vocab.lua)
