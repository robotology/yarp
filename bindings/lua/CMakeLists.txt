# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Arjan Gijsberts, Lorenzo Natale, Fabien Benureau, Stephane Lallee, Ali Paikan, Francesco Romano
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)


set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;${SWIG_COMMON_FLAGS}")
find_package(Lua REQUIRED)

include_directories(SYSTEM ${LUA_INCLUDE_DIR})
swig_add_library(yarp_lua
                 LANGUAGE lua
                 SOURCES ${SWIG_BINDINGS_SOURCE_FILE})
swig_link_libraries(yarp_lua ${LUA_LIBRARY} ${SWIG_YARP_LIBRARIES})

if(NOT MSVC)
  check_cxx_compiler_flag("-Wno-maybe-uninitialized" CXX_HAS_WNO_MAYBE_UNINITIALIZED)
  if(CXX_HAS_WNO_MAYBE_UNINITIALIZED)
    target_compile_options(${SWIG_MODULE_yarp_lua_REAL_NAME} PRIVATE "-Wno-maybe-uninitialized")
  endif()
endif()

set(_CMAKE_INSTALL_LUADIR "${CMAKE_INSTALL_LIBDIR}/lua/${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}")
set(CMAKE_INSTALL_LUADIR ${_CMAKE_INSTALL_LUADIR} CACHE PATH "lua bindings (${_CMAKE_INSTALL_LUADIR})")
mark_as_advanced(CMAKE_INSTALL_LUADIR)
if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_LUADIR})
  set(CMAKE_INSTALL_FULL_LUADIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LUADIR}")
else()
  set(CMAKE_INSTALL_FULL_LUADIR "${CMAKE_INSTALL_LUADIR}")
endif()

set_target_properties(${SWIG_MODULE_yarp_lua_REAL_NAME} PROPERTIES PREFIX ""
                                                                   OUTPUT_NAME "yarp"
                                                                   LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LUADIR})

# Update RPATH
if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
  file(RELATIVE_PATH _rel_path "${CMAKE_INSTALL_FULL_LUADIR}" "${CMAKE_INSTALL_FULL_LIBDIR}")
  get_target_property(_current_rpath "${SWIG_MODULE_yarp_lua_REAL_NAME}" INSTALL_RPATH)
  if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    list(APPEND _current_rpath "@loader_path/${_rel_path}")
  else()
    list(APPEND _current_rpath "\$ORIGIN/${_rel_path}")
  endif()
  set_target_properties(${SWIG_MODULE_yarp_lua_REAL_NAME} PROPERTIES INSTALL_RPATH "${_current_rpath}")
endif()

install(TARGETS ${SWIG_MODULE_yarp_lua_REAL_NAME}
        DESTINATION ${CMAKE_INSTALL_LUADIR})

if(YARP_COMPILE_TESTS)
  add_subdirectory(tests)
endif()
