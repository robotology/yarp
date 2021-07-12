# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(GNUInstallDirs)
include(CMakeDependentOption)
include(YarpRenamedOption)
include(YarpDeprecatedOption)
include(AddInstallRPATHSupport)


#########################################################################
# Control where libraries and executables are placed during the build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")


#########################################################################
# Build configurations
set(YARP_OPTIMIZED_CONFIGURATIONS "Release" "MinSizeRel")
set(YARP_DEBUG_CONFIGURATIONS "Debug" "RelWithDebInfo")

#########################################################################
# DebugFull builds options

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS_DEBUGFULL "-O0 -g3 -ggdb -fno-inline -DDEBUG" CACHE STRING "Flags used by the compiler during debugfull builds.")
  set(CMAKE_CXX_FLAGS_DEBUGFULL "-O0 -g3 -ggdb -fno-inline -DDEBUG" CACHE STRING "Flags used by the compiler during debugfull builds.")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUGFULL "" CACHE STRING "Flags used by the linker during debugfull builds.")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUGFULL "" CACHE STRING "Flags used by the linker during debugfull builds.")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUGFULL "" CACHE STRING "Flags used by the linker during debugfull builds.")
  set(CMAKE_STATIC_LINKER_FLAGS_DEBUGFULL "" CACHE STRING "Flags used by the linker during debugfull builds.")

  mark_as_advanced(CMAKE_C_FLAGS_DEBUGFULL
                   CMAKE_CXX_FLAGS_DEBUGFULL
                   CMAKE_EXE_LINKER_FLAGS_DEBUGFULL
                   CMAKE_MODULE_LINKER_FLAGS_DEBUGFULL
                   CMAKE_SHARED_LINKER_FLAGS_DEBUGFULL
                   CMAKE_STATIC_LINKER_FLAGS_DEBUGFULL)

  list(APPEND YARP_DEBUG_CONFIGURATIONS "DebugFull")
endif()

#########################################################################
# Profile builds options

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS_PROFILE "-pg -g3 -ggdb -fno-inline -ftest-coverage -fprofile-arcs -DNDEBUG" CACHE STRING "Flags used by the compiler during profile builds.")
  set(CMAKE_CXX_FLAGS_PROFILE "-pg -g3 -ggdb -fno-inline -ftest-coverage -fprofile-arcs -DNDEBUG" CACHE STRING "Flags used by the compiler during profile builds.")
  set(CMAKE_EXE_LINKER_FLAGS_PROFILE "" CACHE STRING "Flags used by the linker during profile builds.")
  set(CMAKE_MODULE_LINKER_FLAGS_PROFILE "" CACHE STRING "Flags used by the linker during profile builds.")
  set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "" CACHE STRING "Flags used by the linker during profile builds.")
  set(CMAKE_STATIC_LINKER_FLAGS_PROFILE "" CACHE STRING "Flags used by the linker during profile builds.")

  mark_as_advanced(CMAKE_C_FLAGS_PROFILE
                   CMAKE_CXX_FLAGS_PROFILE
                   CMAKE_EXE_LINKER_FLAGS_PROFILE
                   CMAKE_MODULE_LINKER_FLAGS_PROFILE
                   CMAKE_SHARED_LINKER_FLAGS_PROFILE
                   CMAKE_STATIC_LINKER_FLAGS_PROFILE)

  list(APPEND YARP_DEBUG_CONFIGURATIONS "Profile")
endif()

#########################################################################
# Handle CMAKE_CONFIGURATION_TYPES and CMAKE_BUILD_TYPE

if(NOT CMAKE_CONFIGURATION_TYPES)
  # Possible values for the CMAKE_BUILD_TYPE variable
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${YARP_OPTIMIZED_CONFIGURATIONS} ${YARP_DEBUG_CONFIGURATIONS})
  unset(_configurations)
  foreach(_conf ${YARP_OPTIMIZED_CONFIGURATIONS} ${YARP_DEBUG_CONFIGURATIONS})
    set(_configurations "${_configurations} ${_conf}")
  endforeach()
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY HELPSTRING "Choose the type of build, options are:${_configurations}.")
  if(NOT CMAKE_BUILD_TYPE)
    # Encourage user to specify build type.
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
  endif()
endif()

# Let CMake know which configurations are the debug ones, so that it can
# link the right library when both optimized and debug library are found
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS ${YARP_DEBUG_CONFIGURATIONS})

#########################################################################
# Handle YARP_IDL_BINARY_HINT
set(YARP_IDL_BINARY_HINT "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
foreach(_config ${CMAKE_CONFIGURATION_TYPES})
  list(APPEND YARP_IDL_BINARY_HINT "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${_config}")
endforeach()

#########################################################################
# Simplify compilation of portable binaries.
# To make very portable YARP binaries, put a subdirectory called
# "static_libs" in the build directory, and place any libraries
# (such as libstdc++.a) that should be linked statically there.

set(STATLIB "${CMAKE_BINARY_DIR}/static_libs")
if(EXISTS ${STATLIB})
  message(STATUS "static_libs directory present: ${STATLIB}")
  link_directories(${STATLIB})
  add_definitions(-static-libgcc)
  set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc")
  file(GLOB statlibs ${STATLIB}/*.a)
  link_libraries(${statlibs})
endif()


#########################################################################
# Control whether libraries are shared or static.

option(BUILD_SHARED_LIBS "Compile shared libraries rather than linking statically" ON)
mark_as_advanced(BUILD_SHARED_LIBS)
yarp_renamed_option(CREATE_SHARED_LIBRARY BUILD_SHARED_LIBS) # Since YARP 2.3.68.1

set(YARP_DLL OFF)
if(BUILD_SHARED_LIBS)
  set(YARP_DLL ON)
endif()

set (YARP_LINK_PLUGINS TRUE)
if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
  set (YARP_LINK_PLUGINS FALSE)
endif()


#########################################################################
# Add the option to build only libraries and skip the binaries

option(YARP_COMPILE_EXECUTABLES "Enable YARP executables." ON)


#########################################################################
# Disable unmaintained stuff unless explicitly enabled by the user.

option(YARP_COMPILE_UNMAINTAINED "Enable unmaintained components" OFF)


#########################################################################
# Enable/Disable examples

cmake_dependent_option(YARP_COMPILE_EXAMPLES
                       "Enable YARP examples" OFF
                       "YARP_COMPILE_EXECUTABLES" OFF)


#########################################################################
# Turn on testing.

option(YARP_COMPILE_TESTS "Enable YARP tests" OFF)
if(YARP_COMPILE_TESTS)
  enable_testing()
endif()

cmake_dependent_option(YARP_DISABLE_FAILING_TESTS OFF "Disable tests that fail randomly due to race conditions" YARP_COMPILE_TESTS OFF)
mark_as_advanced(YARP_DISABLE_FAILING_TESTS)

cmake_dependent_option(YARP_ENABLE_BROKEN_TESTS OFF "Enable broken tests" YARP_COMPILE_TESTS OFF)
mark_as_advanced(YARP_ENABLE_BROKEN_TESTS)

cmake_dependent_option(YARP_ENABLE_INTEGRATION_TESTS OFF "Run integration tests" "YARP_COMPILE_TESTS;UNIX" OFF)
mark_as_advanced(YARP_ENABLE_INTEGRATION_TESTS)
yarp_renamed_option(YARP_TEST_INTEGRATION YARP_ENABLE_INTEGRATION_TESTS) # since YARP 3.2.0

cmake_dependent_option(YARP_ENABLE_EXAMPLES_AS_TESTS OFF "Compile examples as unit tests" YARP_COMPILE_TESTS OFF)
mark_as_advanced(YARP_ENABLE_EXAMPLES_AS_TESTS)


#########################################################################
# Test timeout.
set(YARP_TEST_TIMEOUT_DEFAULT 120)
if (APPLE)
  set(YARP_TEST_TIMEOUT_DEFAULT 300)
endif()
set(YARP_TEST_TIMEOUT ${YARP_TEST_TIMEOUT_DEFAULT} CACHE STRING "Timeout for unit tests")
mark_as_advanced(YARP_TEST_TIMEOUT)


#########################################################################
# Run tests under Valgrind

cmake_dependent_option(YARP_VALGRIND_TESTS
                       "Run YARP tests under Valgrind" OFF
                       "YARP_COMPILE_TESTS" OFF)
mark_as_advanced(YARP_VALGRIND_TESTS)

if(YARP_VALGRIND_TESTS)
  find_program(VALGRIND_EXECUTABLE NAMES valgrind)
  mark_as_advanced(VALGRIND_EXECUTABLE)

  if(VALGRIND_EXECUTABLE)
    set(VALGRIND_OPTIONS "--tool=memcheck --leak-check=full"
      CACHE STRING "Valgrind options (--error-exitcode=1 will be appended)")
    mark_as_advanced(VALGRIND_OPTIONS)
    separate_arguments(VALGRIND_OPTIONS UNIX_COMMAND "${VALGRIND_OPTIONS}")
    set(VALGRIND_COMMAND "${VALGRIND_EXECUTABLE}" ${VALGRIND_OPTIONS} --error-exitcode=1 --fullpath-after=${CMAKE_SOURCE_DIR}/)
  else()
    message(SEND_ERROR "Valgrind executable not found")
  endif()
endif()

unset(YARP_TEST_LAUNCHER)
set(YARP_TEST_TIMEOUT_DEFAULT_VALGRIND 300)
if(DEFINED VALGRIND_COMMAND)
  set(YARP_TEST_LAUNCHER ${VALGRIND_COMMAND})
  # The default timeout is not enough when running under valgrind
  if(YARP_TEST_TIMEOUT EQUAL YARP_TEST_TIMEOUT_DEFAULT)
    set_property(CACHE YARP_TEST_TIMEOUT PROPERTY VALUE ${YARP_TEST_TIMEOUT_DEFAULT_VALGRIND})
  endif()
else()
  if(YARP_TEST_TIMEOUT EQUAL YARP_TEST_TIMEOUT_DEFAULT_VALGRIND)
    set_property(CACHE YARP_TEST_TIMEOUT PROPERTY VALUE ${YARP_TEST_TIMEOUT_DEFAULT})
  endif()
endif()

#########################################################################
# Enable these messages for debugging flags

#message(STATUS "WANTED_WARNING_FLAGS = ${WANTED_WARNING_FLAGS}")
#message(STATUS "EXPERIMENTAL_WARNING_FLAGS = ${EXPERIMENTAL_WARNING_FLAGS}")
#message(STATUS "UNWANTED_WARNING_FLAGS = ${UNWANTED_WARNING_FLAGS}")
#message(STATUS "DEPRECATED_DECLARATIONS_FLAGS = ${DEPRECATED_DECLARATIONS_FLAGS}")
#message(STATUS "HARDENING_FLAGS = ${HARDENING_FLAGS}")
#message(STATUS "CXX98_FLAGS = ${CXX98_FLAGS}")
#message(STATUS "CXX11_FLAGS = ${CXX11_FLAGS}")
#message(STATUS "CXX14_FLAGS = ${CXX14_FLAGS}")
#message(STATUS "CXX17_FLAGS = ${CXX17_FLAGS}")


#########################################################################
# Enable/disable wanted, experimental, and unwanted warnings

# FIXME We should fix those warnings and move them to wanted
option(YARP_EXPERIMENTAL_WARNINGS "Enable some more checks that at the moment produce a lot of warnings" OFF)
mark_as_advanced(YARP_EXPERIMENTAL_WARNINGS)
if(YARP_EXPERIMENTAL_WARNINGS)
  set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} ${EXPERIMENTAL_WARNING_FLAGS}")
endif()
set(YARP_CXX_FLAGS "${WANTED_WARNING_FLAGS} ${UNWANTED_WARNING_FLAGS}")


#########################################################################
# Control whether non-public symbols are filtered out

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

option(YARP_EXPERIMENTAL_FILTER_API "Filter out implemementation symbols from the ABI" OFF)
mark_as_advanced(YARP_EXPERIMENTAL_FILTER_API)
if(YARP_EXPERIMENTAL_FILTER_API)
  set(YARP_FILTER_impl ON)
endif()

#########################################################################
# Show warnings for deprecated declarations

set(YARP_CXX_FLAGS "${YARP_CXX_FLAGS} ${DEPRECATED_DECLARATIONS_FLAGS}")


#########################################################################
# Control whether deprecated methods are built and whether they should
# print warnings

option(YARP_NO_DEPRECATED "Filter out deprecated declarations from YARP API" OFF)
mark_as_advanced(YARP_NO_DEPRECATED)
if(YARP_NO_DEPRECATED)
  add_definitions("-DYARP_NO_DEPRECATED")
endif()

cmake_dependent_option(YARP_NO_DEPRECATED_WARNINGS
                       "Do not warn when using YARP deprecated declarations" OFF
                       "NOT YARP_NO_DEPRECATED" OFF)
mark_as_advanced(YARP_NO_DEPRECATED_WARNINGS)
if(YARP_NO_DEPRECATED_WARNINGS)
  add_definitions("-DYARP_NO_DEPRECATED_WARNINGS")
endif()
set(CMAKE_WARN_DEPRECATED ON)
set(CMAKE_ERROR_DEPRECATED OFF)


#########################################################################
# Control whether to build YARP using hardening options

option(YARP_EXPERIMENTAL_HARDENING "Build YARP using hardening flags" OFF)
mark_as_advanced(YARP_EXPERIMENTAL_HARDENING)
if(YARP_EXPERIMENTAL_HARDENING)
  add_definitions("-D_FORTIFY_SOURCE=2")
  set(YARP_CXX_FLAGS "${YARP_CXX_FLAGS} ${HARDENING_FLAGS}")
endif()


#########################################################################
# Control setting an rpath

add_install_rpath_support(LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"       # Libraries
                          BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"       # Binaries
                                   "${CMAKE_INSTALL_FULL_LIBDIR}/yarp"  # Plugins
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)


#########################################################################
# Specify yarp version and copyright into macOS bundles
set(MACOSX_BUNDLE_COPYRIGHT "© 2006-2021 Istituto Italiano di Tecnologia (IIT), 2006-2010 RobotCub Consortium. YARP is released under the terms of the BSD-3-Clause. See the accompanying LICENSE file for details.")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${YARP_VERSION_SHORT}")


#########################################################################
# Compile libraries using -fPIC to produce position independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


#########################################################################
# Control whether build should be aborted on warnings and errors

### -Werror
if(CXX_HAS_WERROR)
  option(YARP_ABORT_ON_WARNING "Consider compiler warnings as errors and abort compilation (-Werror)." OFF)
  mark_as_advanced(YARP_ABORT_ON_WARNING)
  if(YARP_ABORT_ON_WARNING)
    set(YARP_CXX_FLAGS "${YARP_CXX_FLAGS} -Werror")
  endif()
endif()


### -Wfatal-errors
if(CXX_HAS_WFATAL_ERROR)
  option(YARP_FATAL_ERRORS "Abort compilation on the first error rather than trying to keep going and printing further error messages (-Wfatal-errors)" OFF)
  mark_as_advanced(YARP_FATAL_ERRORS)
  if(YARP_FATAL_ERRORS)
    set(YARP_CXX_FLAGS "${YARP_CXX_FLAGS} -Wfatal-errors")
  endif()
endif()


#########################################################################
# Set build flags and be sure to append user defined flags at the end.

get_property(_USER_CMAKE_C_FLAGS CACHE CMAKE_C_FLAGS PROPERTY VALUE)
get_property(_USER_CMAKE_CXX_FLAGS CACHE CMAKE_CXX_FLAGS PROPERTY VALUE)

set(CMAKE_C_FLAGS "${YARP_C_FLAGS} ${_USER_CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${YARP_CXX_FLAGS} ${_USER_CMAKE_CXX_FLAGS}")

# Save the YARP_C_FLAGS and YARP_CXX_FLAGS variables (exported in the
# YARPConfig.cmake file, but remove a few unwanted options from the build
# path that should not be used by other packages.
# -fdebug-prefix-map, -fmacro-prefix-map, -ffile-prefix-map contain the build
# path, and therefore they lead to unreproducible builds.
# See also https://reproducible-builds.org/docs/build-path/
string(REGEX REPLACE "-f(debug|macro|file)-prefix-map=[^ ]+ *" "" YARP_C_FLAGS "${CMAKE_C_FLAGS}")
string(REGEX REPLACE "-f(debug|macro|file)-prefix-map=[^ ]+ *" "" YARP_CXX_FLAGS "${CMAKE_CXX_FLAGS}")


#########################################################################
# Deprecated options

yarp_deprecated_option(ENABLE_FORCE_RPATH) # Since YARP 2.3.65
yarp_deprecated_option(INSTALL_WITH_RPATH) # Since YARP 2.3.65
yarp_deprecated_option(CREATE_BUILTIN_DEVICE_TESTS) # Since YARP 2.3.68
yarp_deprecated_option(YARP_FILTER_API) # Since YARP 2.3.68.1
yarp_deprecated_option(YARP_CLEAN_API) # Since YARP 2.3.68.1
yarp_deprecated_option(YARP_EXPERIMENTAL_CXX11) # Since YARP 3.0.0
yarp_deprecated_option(YARP_WRAP_STL_STRING) # Since YARP 3.0.0
yarp_deprecated_option(YARP_WRAP_STL_STRING_INLINE) # Since YARP 3.0.0
yarp_deprecated_option(YARP_VALGRIND_MEMCHECK_TESTS) # since YARP 3.2.0
yarp_deprecated_option(YARP_VALGRIND_HELGRIND_TESTS) # since YARP 3.2.0
yarp_deprecated_option(YARP_VALGRIND_DRD_TESTS) # since YARP 3.2.0
yarp_deprecated_option(YARP_VALGRIND_SGCHECK_TESTS) # since YARP 3.2.0
yarp_deprecated_option(YARP_GDB_TESTS) # since YARP 3.2.0
yarp_deprecated_option(VALGRIND_MEMCHECK_OPTIONS) # since YARP 3.2.0
yarp_deprecated_option(VALGRIND_HELGRIND_OPTIONS) # since YARP 3.2.0
yarp_deprecated_option(VALGRIND_DRD_OPTIONS) # since YARP 3.2.0
yarp_deprecated_option(VALGRIND_SGCHECK_OPTIONS) # since YARP 3.2.0
yarp_deprecated_option(YARP_GDB_OPTIONS) # since YARP 3.2.0
yarp_deprecated_option(ENABLE_DASHBOARD_SUBMIT) # Since YARP 3.3.3
yarp_deprecated_option(TEST_MACHINE_HOSTNAME) # Since YARP 3.3.3
yarp_deprecated_option(TEST_MACHINE_OS_TYPE) # Since YARP 3.3.3
yarp_deprecated_option(TEST_MACHINE_TEST_TYPE) # Since YARP 3.3.3
