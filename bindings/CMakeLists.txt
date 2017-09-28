# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Arjan Gijsberts, Lorenzo Natale, Fabien Benureau, Stephane Lallee, Ali Paikan, 
#          Francesco Romano, Daniele Domenichelli
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

if(NOT YARP_VERSION)
  if(YARP_NO_DEPRECATED) # since YARP 2.3.72
    message(FATAL_ERROR "Building YARP bindings outside main YARP build folder is not supported")
  endif()
  message(WARNING "Building YARP bindings outside main YARP build folder is deprecated and it will be removed soon")

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
  
  # Find YARP for bindings-only builds
  find_package(YARP REQUIRED)
  find_package(SWIG 3.0 REQUIRED)
endif()

set(YARP_COMPILE_BINDINGS_DEFAULT TRUE)
set(YARP_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
if(YARP_VERSION)
  # this is a nested build
  set(YARP_COMPILE_BINDINGS_DEFAULT FALSE)
  set(YARP_BASE_DIR ${CMAKE_BINARY_DIR})
endif()
option(YARP_COMPILE_BINDINGS "Compile optional language bindings" ${YARP_COMPILE_BINDINGS_DEFAULT})

#############################################################################
## Options for compiling supported languages.  There's nothing magical
## about this list, any SWIG-supported language should work - take a
## look at e.g. ruby code below for how to do it.

set(SUPPORTED_LANGUAGES "Java"
                        "Python"
                        "Perl"
                        "Chicken Scheme"
                        "CSharp"
                        "Allegro Common Lisp"
                        "TCL"
                        "Ruby"
                        "Lua"
                        "Octave")

foreach(Lang ${SUPPORTED_LANGUAGES})
  string(TOUPPER "${Lang}" LANG)
  string(REGEX REPLACE " .+" "" LANG "${LANG}")
  cmake_dependent_option(CREATE_${LANG} "Do you want to create the ${Lang} interface" OFF
                         YARP_COMPILE_BINDINGS OFF)
  if(YARP_COMPILE_BINDINGS)
    if(CREATE_${LANG})
      message(STATUS " +++ bindings ${Lang} is enabled")
    else()
      message(STATUS " --- bindings ${Lang} is not enabled")
    endif()
  endif()
endforeach()

#############################################################################

if (YARP_VERSION AND NOT YARP_NO_DEPRECATED) #since YARP 2.3.72
  # Make sure yarp.i and related source files get installed, to allow
  # bindings for other languages to be compiled from the build material.

  # Install main CMakeLists and Swig input file
  foreach(f CMakeLists.txt yarp.i README.md)
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

if(YARP_COMPILE_BINDINGS)

  #############################################################################
  ## Do not build deprecated functions when disabled

  if(YARP_NO_DEPRECATED)
    set(SWIG_COMMON_FLAGS "-DYARP_NO_DEPRECATED")
  endif()

  #############################################################################
  # Include SWIG use file
  include(${SWIG_USE_FILE})
  
  if(YARP_VERSION)
     get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
   endif()
   # this is necessary for SWIG to parse yarp.i file
  include_directories(${YARP_INCLUDE_DIRS})
  set(SWIG_YARP_LIBRARIES YARP::YARP_OS YARP::YARP_sig YARP::YARP_dev YARP::YARP_init)
  # link_libraries(YARP::YARP_OS YARP::YARP_sig YARP::YARP_dev YARP::YARP_init)

  set(SWIG_BINDINGS_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/yarp.i")

  #############################################################################
  ## Disable deprecated warnings, since we are building bindings also for
  ## deprecated methods

  if(NOT YARP_NO_DEPRECATED)
    if(MSVC)
      # TODO: Move to target_compile_definitions
      add_definitions(/wd4996)
    else()
      check_cxx_compiler_flag("-Wno-deprecated-declarations" CXX_HAS_WNO_DEPRECATED_DECLARATIONS)
      if(CXX_HAS_WNO_DEPRECATED_DECLARATIONS)
        # TODO: Move to target_compile_definitions
        add_definitions(-Wno-deprecated-declarations)
      endif()
    endif()
  endif()


  #############################################################################
  ## Disable suggest-override warnings

  if(NOT MSVC)
    check_cxx_compiler_flag("-Wno-suggest-override" CXX_HAS_WNO_SUGGEST_OVERRIDE)
    if(CXX_HAS_WNO_SUGGEST_OVERRIDE)
      # TODO: Move to target_compile_definitions
      add_definitions(-Wno-suggest-override)
    endif()
  endif()


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
