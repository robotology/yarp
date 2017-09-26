# Copyright: (C) 2017 Istituto Italiano di Tecnologia (IIT)
# Authors: Silvio Traversaro <silvio.traversaro@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# test_string.lua uses the yarpserver, so for now it is not include in the CTest infrastructure

macro(add_lua_unit_test luascript)
  # Find the lua interpreter (ideally, this should be handled in FindLua or in a separate
  # FindLuaInterp like for python)
  # set_tests_properties(... ENVIRONMENT ) is only supported CMake 3.3
  if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.3)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
  endif()
  if(NOT ${CMAKE_VERSION} VERSION_LESS 3.3)
    find_program(LUA_EXECUTABLE lua)
    if(LUA_EXECUTABLE)
      add_test(NAME ${luascript} COMMAND ${LUA_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${luascript})
      set_tests_properties(${luascript} PROPERTIES ENVIRONMENT "LUA_CPATH=\;\;\;$<TARGET_FILE:${target_name}>")
    endif()
  endif()
endmacro()

add_lua_unit_test(test_bottle_and_property.lua)
