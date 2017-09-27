# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Arjan Gijsberts, Lorenzo Natale, Fabien Benureau, Stephane Lallee, Ali Paikan
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 3.0)

include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(GNUInstallDirs)

set(YARP_COMPILE_BINDINGS_DEFAULT TRUE)
set(YARP_COLLISION_AVOIDANCE FALSE)
set(YARP_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
if(YARP_VERSION)
  # this is a nested build
  set(YARP_COMPILE_BINDINGS_DEFAULT FALSE)
  set(YARP_COLLISION_AVOIDANCE TRUE)
  set(YARP_BASE_DIR ${CMAKE_BINARY_DIR})
endif()
option(YARP_COMPILE_BINDINGS "Compile optional language bindings" ${YARP_COMPILE_BINDINGS_DEFAULT})

#############################################################################
## Options for compiling supported languages.  There's nothing magical
## about this list, any SWIG-supported language should work - take a
## look at e.g. ruby code below for how to do it.

foreach(lang "Java"
             "Python"
             "Perl"
             "Chicken Scheme"
             "CSharp"
             "Allegro Common Lisp"
             "TCL"
             "Ruby"
             "Lua"
             "Octave")
  string(TOUPPER "${lang}" LANG)
  string(REGEX REPLACE " .+" "" LANG "${LANG}")
  cmake_dependent_option(CREATE_${LANG} "Do you want to create the ${lang} interface" OFF
                         YARP_COMPILE_BINDINGS OFF)
  if(YARP_COMPILE_BINDINGS)
    if(CREATE_${LANG})
      message(STATUS " +++ bindings ${lang} is enabled")
    else()
      message(STATUS " --- bindings ${lang} is not enabled")
    endif()
  endif()
endforeach()


if(YARP_COMPILE_BINDINGS)

#############################################################################
## Do not build deprecated functions when disabled

if(YARP_NO_DEPRECATED)
  set(SWIG_COMMON_FLAGS "-DYARP_NO_DEPRECATED")
endif()


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

#############################################################################
## Find SWIG and YARP

if (YARP_SPECIAL_SWIG)
  set(__CMAKE_IGNORE_PATH ${CMAKE_IGNORE_PATH})
  set(CMAKE_IGNORE_PATH /usr/bin ${CMAKE_IGNORE_PATH})
  find_package(SWIG REQUIRED)
  set(CMAKE_IGNORE_PATH ${__CMAKE_IGNORE_PATH})
else()
  find_package(SWIG REQUIRED)
endif()
include(${SWIG_USE_FILE})
message(STATUS "${SWIG_USE_FILE}")
set(MIN_SWIG_VERSION "1.3.29")
if("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")
  message(FATAL_ERROR "Please update your SWIG. Current version: ${SWIG_VERSION}. The minimum version needed: ${MIN_SWIG_VERSION}")
endif()

if(NOT YARP_VERSION)
  find_package(YARP REQUIRED)
  list(REMOVE_ITEM YARP_LIBRARIES YARP::YARP_math)
else()
  set(YARP_FOUND TRUE)
  get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
  list(REMOVE_ITEM YARP_LIBRARIES YARP_math)
  get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
endif()
include_directories(${YARP_INCLUDE_DIRS})
link_libraries(${YARP_LIBRARIES})

set(SWIG_BINDINGS_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/yarp.i")
set_source_files_properties(${SWIG_BINDINGS_SOURCE_FILE} PROPERTIES CPLUSPLUS ON)


#############################################################################
## Disable deprecated warnings, since we are building bindings also for
## deprecated methods

if(NOT YARP_NO_DEPRECATED)
  if(MSVC)
    add_definitions(/wd4996)
  else()
    check_cxx_compiler_flag("-Wno-deprecated-declarations" CXX_HAS_WNO_DEPRECATED_DECLARATIONS)
    if(CXX_HAS_WNO_DEPRECATED_DECLARATIONS)
      add_definitions(-Wno-deprecated-declarations)
    endif()
  endif()
endif()


#############################################################################
## Disable suggest-override warnings

if(NOT MSVC)
  check_cxx_compiler_flag("-Wno-suggest-override" CXX_HAS_WNO_SUGGEST_OVERRIDE)
  if(CXX_HAS_WNO_SUGGEST_OVERRIDE)
    add_definitions(-Wno-suggest-override)
  endif()
endif()



#############################################################################
## Create Java bindings

if(CREATE_JAVA)
  add_subdirectory(java)
endif(CREATE_JAVA)



#############################################################################
## Create Python bindings

if(CREATE_PYTHON)
  add_subdirectory(python)
endif(CREATE_PYTHON)



#############################################################################
## Create Perl bindings

if(CREATE_PERL)
  add_subdirectory(perl)
endif(CREATE_PERL)




#############################################################################
## Create Chicken bindings

if(CREATE_CHICKEN)
  add_subdirectory(chicken)
endif(CREATE_CHICKEN)



#############################################################################
## Create Allegro bindings

if(CREATE_ALLEGRO)
  set(CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}")
  set(ALLEGRO_INSTALL_DIR "lib/allegro/site" CACHE PATH "Install path of allegro bindings.")
  set(ALLEGRO_ASDF_SYSTEMS_DIR "lib/allegro/asdf-systems" CACHE PATH "Path to register asdf file.")

  set(allegro_PROJ_FILE allegro/yarp.asd)
  set(allegro_SRC_FILE yarp.cl)
  set(allegro_LIBS yarp-allegro.so)

  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;-identifier-converter;lispify;${SWIG_COMMON_FLAGS}")
  swig_add_library(yarp-allegro
                   LANGUAGE allegrocl
                   SOURCES yarp.i)

  string(REGEX REPLACE "\\.cl" ".lisp" allegro_DEST_FILE ${allegro_SRC_FILE})
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${allegro_PROJ_FILE}
          DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${allegro_SRC_FILE}
          DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp/src
          RENAME ${allegro_DEST_FILE})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${allegro_LIBS}
          DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp/lib)

  if(UNIX)
    install(CODE "execute_process(COMMAND \"\${CMAKE_COMMAND}\" \"-E\" \"make_directory\" \"${ALLEGRO_ASDF_SYSTEMS}\"
                                  COMMAND \"ln\" \"-s\" \"${CMAKE_INSTALL_PREFIX}/${ALLEGRO_INSTALL_DIR}/yarp/yarp.asd\" \"${CMAKE_INSTALL_PREFIX}/${ALLEGRO_ASDF_SYSTEMS}\")")
  endif()
endif()

if(CREATE_TCL)
  add_subdirectory(tcl)
endif(CREATE_TCL)



#############################################################################
## Create C# bindings

if(CREATE_CSHARP)
  add_subdirectory(csharp)
endif(CREATE_CSHARP)



#############################################################################
## Create Ruby bindings

if(CREATE_RUBY)
  add_subdirectory(ruby)
endif(CREATE_RUBY)




#############################################################################
## Create Lua bindings

if(CREATE_LUA)
  add_subdirectory(lua)
endif(CREATE_LUA)

#############################################################################
## Create Octave bindings

if(CREATE_OCTAVE)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;${SWIG_COMMON_FLAGS}")
  find_package(Octave REQUIRED)
  set(target_name yarp_octave)
  get_filename_component(OCTAVE_INCLUDE_TOP ${OCTAVE_INCLUDE_DIR} DIRECTORY)
  include_directories(SYSTEM ${OCTAVE_INCLUDE_DIRS} ${OCTAVE_INCLUDE_TOP})
  swig_add_library(${target_name}
                   LANGUAGE octave
                   SOURCES yarp.i)
  swig_link_libraries(${target_name} ${OCTAVE_LIBRARIES})
  set_target_properties(${target_name} PROPERTIES PREFIX "")
  set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "yarp")
  set_target_properties(${target_name} PROPERTIES SUFFIX ".oct")
  if(YARP_COLLISION_AVOIDANCE)
    set_target_properties(${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/octave)
  endif()
  set(YARP_COLLISION_AVOIDANCE TRUE)
endif(CREATE_OCTAVE)

endif()
