# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


if(NOT YARP_COMPILE_EXAMPLES AND NOT YARP_ENABLE_EXAMPLES_AS_TESTS)
  return()
endif()


# Control where the example libraries and executables are placed during the
# main build.

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/examples")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/examples")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/examples")


# Prepare 'build_generator', 'build_options'
# for tests in subdirectories

set(build_generator --build-generator "${CMAKE_GENERATOR}")
if(CMAKE_GENERATOR_PLATFORM)
  list(APPEND build_generator --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}")
endif()
if(CMAKE_GENERATOR_TOOLSET)
  list(APPEND build_generator --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}")
endif()
if(CMAKE_CONFIGURATION_TYPES)
  list(APPEND build_generator --build-config $<CONFIGURATION>)
endif()

set(build_options)
if(CMAKE_BUILD_TYPE)
  list(APPEND build_options -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
endif()

if(CMAKE_MAKE_PROGRAM)
  list(APPEND build_options -DCMAKE_MAKE_PROGRAM:FILEPATH=${CMAKE_MAKE_PROGRAM})
endif()

list(APPEND build_options -DYARP_DIR=${YARP_BINARY_DIR})
list(APPEND build_options -DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
list(APPEND build_options -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS})
list(APPEND build_options -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS})
list(APPEND build_options -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/examples)
list(APPEND build_options -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/examples)
list(APPEND build_options -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/examples)


# Add example subdirectories

add_subdirectory(os)
add_subdirectory(sig)
add_subdirectory(dev)
