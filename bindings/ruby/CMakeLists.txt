# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)

find_package(Ruby REQUIRED)
set(CMAKE_SWIG_FLAGS "-module;yarp;${SWIG_COMMON_FLAGS}")


swig_add_library(yarp_ruby
                 LANGUAGE ruby
                 SOURCES ${SWIG_BINDINGS_SOURCE_FILE})
swig_link_libraries(yarp_ruby ${RUBY_LIBRARY} ${SWIG_YARP_LIBRARIES})
target_include_directories(${SWIG_MODULE_yarp_ruby_REAL_NAME} SYSTEM PRIVATE ${RUBY_INCLUDE_DIRS})

set_target_properties(${SWIG_MODULE_yarp_ruby_REAL_NAME} PROPERTIES PREFIX ""
                                                                    OUTPUT_NAME "yarp")

# Define CMAKE_INSTALL_RUBYDIR for installing Ruby libraries
function(_RUBY_CONFIG_VAR RBVAR OUTVAR)
  execute_process(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print RbConfig::CONFIG['${RBVAR}']"
    RESULT_VARIABLE _RUBY_SUCCESS
    OUTPUT_VARIABLE _RUBY_OUTPUT
    ERROR_QUIET)
  if(_RUBY_SUCCESS OR _RUBY_OUTPUT STREQUAL "")
    execute_process(COMMAND ${RUBY_EXECUTABLE} -r rbconfig -e "print Config::CONFIG['${RBVAR}']"
      RESULT_VARIABLE _RUBY_SUCCESS
      OUTPUT_VARIABLE _RUBY_OUTPUT
      ERROR_QUIET)
  endif()
  set(${OUTVAR} "${_RUBY_OUTPUT}" PARENT_SCOPE)
endfunction()

if(NOT DEFINED CMAKE_INSTALL_RUBYDIR)
  _RUBY_CONFIG_VAR("vendorarchdir" RUBY_VENDORARCH_DIR)
  _RUBY_CONFIG_VAR("archlibdir" RUBY_ARCHLIB_DIR)
  string(REGEX REPLACE "^${RUBY_ARCHLIB_DIR}" "" _rubydir "${RUBY_VENDORARCH_DIR}")
  set(CMAKE_INSTALL_RUBYDIR "${CMAKE_INSTALL_LIBDIR}${_rubydir}" CACHE PATH "Ruby libraries (${_rubydir})")
endif()

mark_as_advanced(CMAKE_INSTALL_RUBYDIR)
if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_RUBYDIR})
  set(CMAKE_INSTALL_FULL_RUBYDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_RUBYDIR}")
else()
  set(CMAKE_INSTALL_FULL_RUBYDIR "${CMAKE_INSTALL_RUBYDIR}")
endif()

set_target_properties(${SWIG_MODULE_yarp_ruby_REAL_NAME} PROPERTIES PREFIX ""
                                                                    OUTPUT_NAME "yarp"
                                                                    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_RUBYDIR})

# Update RPATH
if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
  file(RELATIVE_PATH _rel_path "${CMAKE_INSTALL_FULL_RUBYDIR}" "${CMAKE_INSTALL_FULL_LIBDIR}")
  get_target_property(_current_rpath "${SWIG_MODULE_yarp_ruby_REAL_NAME}" INSTALL_RPATH)
  if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    list(APPEND _current_rpath "@loader_path/${_rel_path}")
  else()
    list(APPEND _current_rpath "\$ORIGIN/${_rel_path}")
  endif()
  set_target_properties(${SWIG_MODULE_yarp_ruby_REAL_NAME} PROPERTIES INSTALL_RPATH "${_current_rpath}")
endif()

install(TARGETS ${SWIG_MODULE_yarp_ruby_REAL_NAME}
        DESTINATION ${CMAKE_INSTALL_RUBYDIR})

if(YARP_COMPILE_TESTS)
  add_subdirectory(tests)
endif()
