# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

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

if(BUILD_SHARED_LIBS)
  set(YARP_DLL ON)
endif()


#########################################################################
# Add the option to build only libraries and skip the binaries

option(YARP_COMPILE_EXECUTABLES "Enable YARP executables." ON)
mark_as_advanced(YARP_COMPILE_EXECUTABLES)


#########################################################################
# Add the option to build Robot Testing Framework addons
option(YARP_COMPILE_RTF_ADDONS "Compile Robot Testing Framework addons." OFF)
mark_as_advanced(YARP_COMPILE_RTF_ADDONS)


#########################################################################
# Disable unmaintained stuff unless explicitly enabled by the user.

option(YARP_COMPILE_UNMAINTAINED "Enable unmaintained components" OFF)
mark_as_advanced(YARP_COMPILE_UNMAINTAINED)


#########################################################################
# Turn on testing.

option(YARP_COMPILE_TESTS "Enable YARP tests" OFF)
mark_as_advanced(YARP_COMPILE_TESTS)
if(YARP_COMPILE_TESTS)
  enable_testing()
endif()

#########################################################################
# Run tests under Valgrind

yarp_deprecated_option(VALGRIND_OPTIONS)
cmake_dependent_option(YARP_VALGRIND_TESTS
                       "Run YARP tests under Valgrind" OFF
                       "YARP_COMPILE_TESTS" OFF)
mark_as_advanced(YARP_VALGRIND_TESTS)

if(YARP_VALGRIND_TESTS)
  find_program(VALGRIND_EXECUTABLE NAMES valgrind)
  mark_as_advanced(VALGRIND_EXECUTABLE)

  set(YARP_VALGRIND_TOOLS MemCheck
                          Helgrind
                          DRD
                          SGCheck)

  set(YARP_VALGRIND_MEMCHECK_TESTS_DEFAULT ON)
  set(YARP_VALGRIND_HELGRIND_TESTS_DEFAULT OFF)
  set(YARP_VALGRIND_DRD_TESTS_DEFAULT OFF)
  set(YARP_VALGRIND_SGCHECK_TESTS_DEFAULT OFF)

  set(VALGRIND_MEMCHECK_OPTIONS_DEFAULT "--leak-check=full")
  set(VALGRIND_HELGRIND_OPTIONS_DEFAULT "")
  set(VALGRIND_DRD_OPTIONS_DEFAULT "")
  set(VALGRIND_SGCHECK_OPTIONS_DEFAULT "")

  foreach(_Tool ${YARP_VALGRIND_TOOLS})
    string(TOUPPER "${_Tool}" _TOOL)
    string(TOLOWER "${_Tool}" _tool)
    if("${_tool}" MATCHES "sgcheck")
      set(_tool "exp-${_tool}")
    endif()

    cmake_dependent_option(YARP_VALGRIND_${_TOOL}_TESTS
                            "Run YARP tests under Valgrind ${_Tool} tool" ${YARP_VALGRIND_${_TOOL}_TESTS_DEFAULT}
                            "YARP_VALGRIND_TESTS" OFF)

    if(VALGRIND_EXECUTABLE)
      set(VALGRIND_${_TOOL}_OPTIONS "${VALGRIND_${_TOOL}_OPTIONS_DEFAULT}"
          CACHE STRING "Valgrind ${_Tool} tool options (--error-exitcode=1 will be appended)")
      separate_arguments(VALGRIND_${_TOOL}_OPTIONS UNIX_COMMAND "${VALGRIND_${_TOOL}_OPTIONS}")
      set(VALGRIND_${_TOOL}_COMMAND "${VALGRIND_EXECUTABLE}" --tool=${_tool} ${VALGRIND_${_TOOL}_OPTIONS} --error-exitcode=1 --fullpath-after=${CMAKE_SOURCE_DIR}/)
      mark_as_advanced(VALGRIND_${_TOOL}_OPTIONS)
    else()
      message(WARNING "Valgrind not found. Cannot enable ${_Tool} tests.")
      unset(VALGRIND_${_TOOL}_COMMAND)
    endif()
  endforeach()
endif()

#########################################################################
# Run tests under gdb

cmake_dependent_option(YARP_GDB_TESTS
                       "Run YARP tests under gdb" OFF
                       "YARP_COMPILE_TESTS" OFF)
mark_as_advanced(YARP_GDB_TESTS)

if(YARP_GDB_TESTS)
  find_program(GDB_EXECUTABLE NAMES gdb)
  mark_as_advanced(GDB_EXECUTABLE)
  set(GDB_OPTIONS_DEFAULT "")
  if(GDB_EXECUTABLE)
    set(GDB_OPTIONS "${GDB_OPTIONS_DEFAULT}"
        CACHE STRING "gdb options (--return-child-result will be appended)")
    separate_arguments(GDB_OPTIONS UNIX_COMMAND "${GDB_OPTIONS}")
    set(GDB_COMMAND "${GDB_EXECUTABLE}" -batch ${GDB_OPTIONS} -ex "run" -ex "bt" --return-child-result --args)
  endif()
endif()


#########################################################################
# Enable these messages for debugging flags

#message(STATUS "WANTED_WARNING_FLAGS = ${WANTED_WARNING_FLAGS}")
#message(STATUS "EXPERIMENTAL_WARNING_FLAGS = ${EXPERIMENTAL_WARNING_FLAGS}")
#message(STATUS "UNWANTED_WARNING_FLAGS = ${UNWANTED_WARNING_FLAGS}")
#message(STATUS "VISIBILITY_HIDDEN_FLAGS = ${VISIBILITY_HIDDEN_FLAGS}")
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WANTED_WARNING_FLAGS} ${UNWANTED_WARNING_FLAGS}")


#########################################################################
# Control whether non-public symbols are filtered out

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VISIBILITY_HIDDEN_FLAGS}")

option(YARP_EXPERIMENTAL_FILTER_API "Filter out implemementation symbols from the ABI" OFF)
mark_as_advanced(YARP_EXPERIMENTAL_FILTER_API)
if(YARP_EXPERIMENTAL_FILTER_API)
  set(YARP_FILTER_impl ON)
endif()
yarp_deprecated_option(YARP_FILTER_API) # Since YARP 2.3.68.1
yarp_deprecated_option(YARP_CLEAN_API) # Since YARP 2.3.68.1

#########################################################################
# Show warnings for deprecated declarations

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEPRECATED_DECLARATIONS_FLAGS}")


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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HARDENING_FLAGS}")
endif()


#########################################################################
# Ensure that it is compiled with c++11 also with CMake 3.1

if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
  message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
endif()
if(${CMAKE_VERSION} VERSION_LESS 3.1)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")
endif()


#########################################################################
# Control whether to build YARP using C++11 replacements for ACE stuff

option(YARP_EXPERIMENTAL_CXX11 "Build YARP using C++11 replacement for threads, semaphores, etc." OFF)
mark_as_advanced(YARP_EXPERIMENTAL_CXX11)
if(YARP_EXPERIMENTAL_CXX11)
  set(YARP_HAS_CXX11 ON)
endif()


#########################################################################
# Control whether yarp::os::ConstString should be std::string or opaque
# Not an important option for end users yet.  In principle
# yarp::os::ConstString could now be set to std::string, if YARP
# ever decides to accept STL as a dependency.

set(YARP_WRAP_STL_STRING_DEFAULT OFF)
if(MSVC)
  set(YARP_WRAP_STL_STRING_DEFAULT ON)
endif()
option(YARP_WRAP_STL_STRING "Do you want the yarp string classes to wrap std::string? (as opposed to being exactly std::string)" ${YARP_WRAP_STL_STRING_DEFAULT})
mark_as_advanced(YARP_WRAP_STL_STRING)
set(YARP_WRAP_STL_STRING_INLINE_DEFAULT ON)
if(MSVC)
  set(YARP_WRAP_STL_STRING_INLINE_DEFAULT OFF)
endif()
cmake_dependent_option(YARP_WRAP_STL_STRING_INLINE "If wrapping std::string, should we use an inline implementation? (as opposed to opaque)" ${YARP_WRAP_STL_STRING_INLINE_DEFAULT}
                       YARP_WRAP_STL_STRING OFF)
mark_as_advanced(YARP_WRAP_STL_STRING_INLINE)


#########################################################################
# Control compilation of device tests.
# Not really for end-user, but instead for the library developers
yarp_deprecated_option(CREATE_BUILTIN_DEVICE_TESTS) # Since YARP 2.3.68


#########################################################################
# Control submission of reports
option(ENABLE_DASHBOARD_SUBMIT "Allow submission of builds to http://dashboard.icub.org/index.php?project=YARP" OFF)
mark_as_advanced(ENABLE_DASHBOARD_SUBMIT)
if(ENABLE_DASHBOARD_SUBMIT)
  include(CTest)
endif()


#########################################################################
# Control setting an rpath
yarp_deprecated_option(ENABLE_FORCE_RPATH)
yarp_deprecated_option(INSTALL_WITH_RPATH)
add_install_rpath_support(LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"       # Libraries
                          BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"       # Binaries
                                   "${CMAKE_INSTALL_FULL_LIBDIR}/yarp"  # Plugins
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

#########################################################################
# Specify yarp version and copyright into macOS bundles
set(MACOSX_BUNDLE_COPYRIGHT "© 2006-2018 Istituto Italiano di Tecnologia (IIT), 2006-2010 RobotCub Consortium. YARP is released under the terms of the BSD-3-Clause. See the accompanying LICENSE file for details.")
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()
endif()


### -Wfatal-errors
if(CXX_HAS_WFATAL_ERROR)
  option(YARP_FATAL_ERRORS "Abort compilation on the first error rather than trying to keep going and printing further error messages (-Wfatal-errors)" OFF)
  mark_as_advanced(YARP_FATAL_ERRORS)
  if(YARP_FATAL_ERRORS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")
  endif()
endif()


#########################################################################
# Display test machine options for reference, if they are set

if(TEST_MACHINE_HOSTNAME)
  message(STATUS "TEST_MACHINE_HOSTNAME: ${TEST_MACHINE_HOSTNAME}")
  message(STATUS "TEST_MACHINE_OS_TYPE: ${TEST_MACHINE_OS_TYPE}")
  message(STATUS "TEST_MACHINE_TEST_TYPE: ${TEST_MACHINE_TEST_TYPE}")
endif()
