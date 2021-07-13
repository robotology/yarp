# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# this is set in the parent directory, but for some reason it does not get inherited
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)

set(CMAKE_SWIG_FLAGS "-module;yarp;-threads;${SWIG_COMMON_FLAGS}")

# https://github.com/pybind/pybind11/pull/2689
if(${CMAKE_VERSION} VERSION_LESS "3.18.0")
  set(YARP_Python_Development_Component "Development")
else()
  set(YARP_Python_Development_Component "Development.Module")
endif()

find_package(Python3 COMPONENTS Interpreter ${YARP_Python_Development_Component} REQUIRED)

set(CMAKE_SWIG_OUTDIR "${CMAKE_BINARY_DIR}/lib/python3")
set(SWIG_OUTFILE_DIR "${CMAKE_CURRENT_BINARY_DIR}")

if(${CMAKE_VERSION} VERSION_LESS "3.15.0")
  set(YARP_Python_Module_Target "Python3::Python")
else()
  set(YARP_Python_Module_Target "Python3::Module")
endif()

swig_add_library(yarp_python
  LANGUAGE python
  SOURCES ${SWIG_BINDINGS_SOURCE_FILE}
)
swig_link_libraries(yarp_python ${YARP_Python_Module_Target} ${SWIG_YARP_LIBRARIES})

set_target_properties(${SWIG_MODULE_yarp_python_REAL_NAME}
  PROPERTIES
    OUTPUT_NAME "_yarp"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/python3"
)

# installation path is determined reliably on most platforms using distutils
execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "from distutils import sysconfig; print(sysconfig.get_python_lib(plat_specific=True,standard_lib=False,prefix=''))"
  OUTPUT_VARIABLE Python3_INSTDIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(_CMAKE_INSTALL_PYTHON3DIR "${Python3_INSTDIR}")
set(CMAKE_INSTALL_PYTHON3DIR ${_CMAKE_INSTALL_PYTHON3DIR} CACHE PATH "python3 bindings (${_CMAKE_INSTALL_PYTHON3DIR})")
mark_as_advanced(CMAKE_INSTALL_PYTHON3DIR)
if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_PYTHON3DIR})
  set(CMAKE_INSTALL_FULL_PYTHON3DIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_PYTHON3DIR}")
else()
  set(CMAKE_INSTALL_FULL_PYTHON3DIR "${CMAKE_INSTALL_PYTHON3DIR}")
endif()

install(FILES ${CMAKE_BINARY_DIR}/lib/python3/yarp.py
        DESTINATION ${CMAKE_INSTALL_PYTHON3DIR})


# Update RPATH
if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
  file(RELATIVE_PATH _rel_path "${CMAKE_INSTALL_FULL_PYTHON3DIR}" "${CMAKE_INSTALL_FULL_LIBDIR}")
  get_target_property(_current_rpath ${SWIG_MODULE_yarp_python_REAL_NAME} INSTALL_RPATH)
  if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    list(APPEND _current_rpath "@loader_path/${_rel_path}")
  else()
    list(APPEND _current_rpath "\$ORIGIN/${_rel_path}")
  endif()
  set_target_properties(${SWIG_MODULE_yarp_python_REAL_NAME} PROPERTIES INSTALL_RPATH "${_current_rpath}")
endif()

install(
  TARGETS ${SWIG_MODULE_yarp_python_REAL_NAME}
  DESTINATION ${CMAKE_INSTALL_PYTHON3DIR}
)

if(YARP_COMPILE_TESTS)
  add_subdirectory(tests)
endif()

yarp_deprecated_option(CREATE_PYTHON_VERSION)
yarp_deprecated_option(YARP_USE_PYTHON_VERSION)
