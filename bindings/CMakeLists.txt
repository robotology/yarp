# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

if(NOT DEFINED CMAKE_MINIMUM_REQUIRED_VERSION)
  cmake_minimum_required(VERSION 3.12)

  # UseSWIG generates now standard target names.
  if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.13)
    cmake_policy(SET CMP0078 OLD)
  endif()

  # UseSWIG honors SWIG_MODULE_NAME via -module flag.
  if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.14)
    cmake_policy(SET CMP0086 OLD)
  endif()

  project(yarp-bindings)

  #############################################################################
  ## Discourage inplace compilation

  if(CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    option(COMPILE_INPLACE "Allow inplace compilation" FALSE)
    if(NOT COMPILE_INPLACE)
      if(NOT BEND_OVER_BACKWARDS_COMPATIBLE)
        message(FATAL_ERROR "Please don't compile bindings in the source directory, make a separate build directory (ideally one per language).  If CMake has now added a CMakeCache.txt file in the source directory, please delete it.  If you really really want to compile in place, set the COMPILE_INPLACE flag.")
      endif()
    endif()
  endif()

  # Find YCM, YARP and SWIG for bindings-only builds
  set(YCM_REQUIRED_VERSION 0.11.0)
  find_package(YCM ${YCM_REQUIRED_VERSION} REQUIRED)
  find_package(YARP COMPONENTS ${_components} REQUIRED)
  find_package(SWIG 3.0.12 REQUIRED)

  set(_nested_build 0)
  set(YARP_COMPILE_BINDINGS_DEFAULT TRUE)
  set(YARP_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
else()
  set(_nested_build 1)
  set(YARP_COMPILE_BINDINGS_DEFAULT FALSE)
  set(YARP_BASE_DIR ${CMAKE_BINARY_DIR})
endif()

include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

# disable all warnings for current folder and subfolders
# (unless explicitly added by the user)
get_property(_USER_CMAKE_C_FLAGS CACHE CMAKE_C_FLAGS PROPERTY VALUE)
get_property(_USER_CMAKE_CXX_FLAGS CACHE CMAKE_CXX_FLAGS PROPERTY VALUE)
set(CMAKE_C_FLAGS "-w ${_USER_CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-w ${_USER_CMAKE_CXX_FLAGS}")

set(_components conf os sig dev)

# This is necessary for SWIG to parse yarp.i file
# TODO This might no longer be necessary with some updated UseSWIG.cmake version
foreach(_component IN LISTS _components)
  get_property(_include_dirs TARGET YARP::YARP_${_component} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
  foreach(_dir IN LISTS _include_dirs)
    if("${_dir}" MATCHES "\\\$<BUILD_INTERFACE:(.+)>$")
      include_directories("${CMAKE_MATCH_1}")
    elseif("${_dir}" MATCHES "\\\$<INSTALL_INTERFACE:(.+)>$")
      # Nothing to do
    else()
      include_directories(${_dir})
    endif()
  endforeach()
endforeach()

include(${YARP_MODULE_DIR}/YarpPrintFeature.cmake)
include(${YARP_MODULE_DIR}/YarpDeprecatedOption.cmake)

option(YARP_COMPILE_BINDINGS "Compile optional language bindings" ${YARP_COMPILE_BINDINGS_DEFAULT})
yarp_print_feature(YARP_COMPILE_BINDINGS 0 "Bindings")

#############################################################################
## Options for compiling supported languages.  There's nothing magical
## about this list, any SWIG-supported language should work - take a
## look at e.g. ruby code below for how to do it.

set(SUPPORTED_LANGUAGES "Java"
                        "Python"
                        "Perl"
                        "CSharp"
                        "TCL"
                        "Ruby"
                        "Lua"
                        "Octave")

foreach(Lang ${SUPPORTED_LANGUAGES})
  string(TOUPPER "${Lang}" LANG)
  string(REGEX REPLACE " .+" "" LANG "${LANG}")
  cmake_dependent_option(CREATE_${LANG} "Do you want to create the ${Lang} interface" OFF
                         YARP_COMPILE_BINDINGS OFF)
  yarp_print_feature(CREATE_${LANG} 1 "${Lang} bindings")
endforeach()

yarp_deprecated_option("CREATE_CHICKEN") # Since YARP 3.2
yarp_deprecated_option("CREATE_ALLEGRO") # Since YARP 3.2


#############################################################################

if (_nested_build)
  # Make sure yarp.i and related source files get installed, to allow
  # bindings for other languages to be compiled from the build material.

  # Install main CMakeLists and Swig input file
  foreach(f CMakeLists.txt yarp.i macrosForMultipleAnalogSensors.i matlab/vectors_fromTo_matlab.i README.md)
      install(FILES ${CMAKE_SOURCE_DIR}/bindings/${f}
              COMPONENT development
              DESTINATION ${CMAKE_INSTALL_DATADIR}/yarp/bindings)
  endforeach(f)

  # Install supported languages data
  foreach(Lang ${SUPPORTED_LANGUAGES})
    string(TOLOWER "${Lang}" lang)
    string(REGEX REPLACE " .+" "" lang "${lang}")

    install(DIRECTORY ${CMAKE_SOURCE_DIR}/bindings/${lang}
            COMPONENT development
            DESTINATION ${CMAKE_INSTALL_DATADIR}/yarp/bindings)
  endforeach()

endif()

#############################################################################
## Allow passing extra flags to swig (e.g. -Wall)

set(SWIG_EXTRA_FLAGS "" CACHE STRING "Extra flags passed to swig commands (e.g. -Wall)")
mark_as_advanced(SWIG_EXTRA_FLAGS)

#############################################################################

if(YARP_COMPILE_BINDINGS)
  unset(SWIG_COMMON_FLAGS)

  #############################################################################
  ## Do not build deprecated functions when disabled

  if(YARP_NO_DEPRECATED)
    list(APPEND SWIG_COMMON_FLAGS "-DYARP_NO_DEPRECATED")
  endif()

  # Append user flags
  list(APPEND SWIG_COMMON_FLAGS ${SWIG_EXTRA_FLAGS})

  #############################################################################
  # Include SWIG use file
  include(${SWIG_USE_FILE})

  set(SWIG_YARP_LIBRARIES YARP::YARP_os
                          YARP::YARP_sig
                          YARP::YARP_dev
                          YARP::YARP_init)

  set(SWIG_BINDINGS_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/yarp.i")

  #############################################################################
  # Create bindings for enabled languages

  foreach(Lang ${SUPPORTED_LANGUAGES})
    string(REGEX REPLACE " .+" "" trimmed_lang "${Lang}")
    string(TOUPPER ${trimmed_lang} LANG)
    string(TOLOWER ${trimmed_lang} lang)

    if(CREATE_${LANG})
      add_subdirectory(${lang})
    endif()
  endforeach()

endif()
