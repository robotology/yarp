# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Arjan Gijsberts, Lorenzo Natale, Fabien Benureau, Stephane Lallee
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.7)

set (YARP_COMPILE_BINDINGS_DEFAULT TRUE)
set (YARP_COLLISION_AVOIDANCE FALSE)
set (YARP_BASE_DIR ${CMAKE_CURRENT_BINARY_DIR})
if (YARP_VERSION)
  # this is a nested build
  set (YARP_COMPILE_BINDINGS_DEFAULT FALSE)
  set (YARP_COLLISION_AVOIDANCE TRUE)
  set (YARP_BASE_DIR ${CMAKE_BINARY_DIR})
endif ()
option(YARP_COMPILE_BINDINGS "Compile optional language bindings" ${YARP_COMPILE_BINDINGS_DEFAULT})

if (YARP_COMPILE_BINDINGS)

#############################################################################
## Discourage inplace compilation

if (CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  option(COMPILE_INPLACE "Allow inplace compilation" FALSE)
  if (NOT COMPILE_INPLACE)
    if (NOT BEND_OVER_BACKWARDS_COMPATIBLE)
      message(FATAL_ERROR "Please don't compile bindings in the source directory, make a separate build directory (ideally one per language).  If CMake has now added a CMakeCache.txt file in the source directory, please delete it.  If you really really want to compile in place, set the COMPILE_INPLACE flag.")
    endif ()
  endif ()
endif ()



#############################################################################
## Options for compiling supported languages.  There's nothing magical
## about this list, any SWIG-supported language should work - take a
## look at e.g. ruby code below for how to do it.

SET(CREATE_JAVA FALSE CACHE BOOL "Do you want to create a Java interface")
SET(CREATE_PYTHON FALSE CACHE BOOL "Do you want to create the Python interface")
SET(CREATE_PERL FALSE CACHE BOOL "Do you want to create the Perl interface")
SET(CREATE_CHICKEN FALSE CACHE BOOL "Do you want to create the Chicken Scheme interface")
SET(CREATE_CSHARP FALSE CACHE BOOL "Do you want to create the CSharp interface")
SET(CREATE_ALLEGRO FALSE CACHE BOOL "Do you want to create the Allegro Common Lisp interface")
SET(CREATE_TCL FALSE CACHE BOOL "Do you want to create the TCL interface")
SET(CREATE_RUBY FALSE CACHE BOOL "Do you want to create the Ruby interface")
SET(CREATE_LUA FALSE CACHE BOOL "Do you want to create the Lua interface")



#############################################################################
## Find SWIG and YARP

FIND_PACKAGE(SWIG REQUIRED)
INCLUDE(${SWIG_USE_FILE})
MESSAGE(STATUS "${SWIG_USE_FILE}")
SET (MIN_SWIG_VERSION "1.3.29")
IF ("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")
  MESSAGE( FATAL_ERROR "Please update your SWIG. Current version: ${SWIG_VERSION}. The minimum version needed: ${MIN_SWIG_VERSION}")
ENDIF ("${SWIG_VERSION}" VERSION_LESS "${MIN_SWIG_VERSION}")

if (NOT YARP_VERSION)
  find_package(YARP REQUIRED)
else ()
  set(YARP_FOUND TRUE)
  get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
  get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
endif ()
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

SET_SOURCE_FILES_PROPERTIES(yarp.i PROPERTIES CPLUSPLUS ON)



#############################################################################
## Create Java bindings

IF (CREATE_JAVA)
  OPTION(PREPARE_CLASS_FILES "Prepare class files" OFF)
  SET(YARP_HELPERS LoadYarp.java YarpImageHelper.java Example1.java)
  MAKE_DIRECTORY("${YARP_BASE_DIR}/yarp")
  SET(CMAKE_SWIG_OUTDIR_SAVE ${CMAKE_SWIG_OUTDIR})
  SET(CMAKE_SWIG_OUTDIR "${YARP_BASE_DIR}/yarp")
  SET(TARGET_DIR src)
  IF (NOT BINDINGS_SRC_DIR)
    SET(BINDINGS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
  ENDIF ()
  IF (NOT PREPARE_CLASS_FILES)
    SET(CMAKE_SWIG_OUTDIR "${YARP_BASE_DIR}/generated_src")
    SET(TARGET_DIR generated_src)
  ENDIF ()
  MAKE_DIRECTORY("${YARP_BASE_DIR}/${TARGET_DIR}")
  FOREACH(src ${YARP_HELPERS})
    SET(target ${YARP_BASE_DIR}/${TARGET_DIR}/${src})
    CONFIGURE_FILE(${BINDINGS_SRC_DIR}/${src}
      ${target} IMMEDIATE)
    MESSAGE(STATUS "copied ${target}")
  ENDFOREACH(src)
  SET(CMAKE_SWIG_FLAGS "-package;yarp;-Wall;-module;yarp")
  FIND_PACKAGE(JNI REQUIRED)
  INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} )
  SET(target_name jyarp)
  SWIG_ADD_MODULE(${target_name} java yarp.i)
  IF (PREPARE_CLASS_FILES)
    file(GLOB JAVA_IN_SRC RELATIVE ${YARP_BASE_DIR} ${YARP_BASE_DIR}/src/*.java)
    file(GLOB JAVA_IN_YARP RELATIVE ${YARP_BASE_DIR} ${YARP_BASE_DIR}/yarp/*.java)
    set(CLASS_FILES)
    foreach(java_file ${JAVA_IN_SRC} ${JAVA_IN_YARP})
      string(REPLACE ".java" ".class" class_file ${java_file})
      list(APPEND CLASS_FILES ${YARP_BASE_DIR}/${class_file})
    endforeach()
    add_custom_command(OUTPUT ${CLASS_FILES}
      COMMAND javac -source 1.3 -target 1.3 "src/*.java" "yarp/*.java"
      WORKING_DIRECTORY ${YARP_BASE_DIR}
      DEPENDS ${target_name})
    add_custom_target(${target_name}_classes ALL DEPENDS ${CLASS_FILES})
  ENDIF ()

  # issue on MINGW where C++ name-mangling prevents java finding methods
  IF (MINGW)
    MESSAGE(STATUS "untested MINGW patch - see CMakeLists.txt")
    SET_TARGET_PROPERTIES(${target_name} PROPERTIES 
      LINK_FLAGS "--add-stdcall-alias")
  ENDIF (MINGW)
  ## the following is based on:
  ## http://www.cmake.org/pipermail/cmake/2007-September/016343.html
  IF(APPLE)
    SET_TARGET_PROPERTIES(${target_name} PROPERTIES SUFFIX ".jnilib")
  ENDIF(APPLE)
  set (YARP_COLLISION_AVOIDANCE TRUE)
  set(CMAKE_SWIG_OUTDIR ${CMAKE_SWIG_OUTDIR_SAVE})
ENDIF (CREATE_JAVA)



#############################################################################
## Create Python bindings

IF (CREATE_PYTHON)	 
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;-threads")
  SET(CREATE_PYTHON_VERSION "" CACHE STRING "Control python version used, if you wish." )
  if (CREATE_PYTHON_VERSION)
    set(PythonLibs_FIND_VERSION ${CREATE_PYTHON_VERSION})
  endif ()
  FIND_PACKAGE(PythonLibs ${CREATE_PYTHON_VERSION})
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
  LINK_LIBRARIES(${PYTHON_LIBRARY})
  
  ## in Windows it seems necessary to declare explictly the link_directory
  if (WIN32)
    get_filename_component(PYTHON_DIR ${PYTHON_LIBRARY} PATH)
    link_directories(${PYTHON_DIR})
  endif()
  
  set (target_name yarp)
  if (YARP_COLLISION_AVOIDANCE)
    set (target_name yarp_python)
  endif ()
  SWIG_ADD_MODULE(${target_name} python yarp.i)
  # uncomment if we want a debug version of the library
  #ADD_DEFINITIONS(-DPy_DEBUG)

  set_target_properties(_${target_name} PROPERTIES OUTPUT_NAME "_yarp")
  if (YARP_COLLISION_AVOIDANCE)
    set_target_properties(_${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY 
      ${CMAKE_BINARY_DIR}/lib/python)
  endif ()
  GET_TARGET_PROPERTY(yarp_lib_location _${target_name} LOCATION)

  # installation path is determined reliably on most platforms using distutils
  FIND_PACKAGE(PythonInterp ${CREATE_PYTHON_VERSION})

  EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} 
    -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix='${CMAKE_INSTALL_PREFIX}'))" 
    OUTPUT_VARIABLE PYTHON_INSTDIR OUTPUT_STRIP_TRAILING_WHITESPACE )
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/yarp.py ${yarp_lib_location} 
    DESTINATION ${PYTHON_INSTDIR} )

  IF (WIN32)
    SET_TARGET_PROPERTIES(_${target_name} PROPERTIES SUFFIX ".pyd")
  ENDIF (WIN32)
  set (YARP_COLLISION_AVOIDANCE TRUE)
ENDIF (CREATE_PYTHON)



#############################################################################
## Create Perl bindings

IF (CREATE_PERL)
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  FIND_PACKAGE(PerlLibs)
  INCLUDE_DIRECTORIES(${PERL_INCLUDE_PATH})
  SWIG_ADD_MODULE(libperlyarp perl5 yarp.i)
  set (YARP_COLLISION_AVOIDANCE TRUE)
ENDIF (CREATE_PERL)




#############################################################################
## Create Chicken bindings

IF (CREATE_CHICKEN)
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;cyarp")
  SWIG_ADD_MODULE(libcyarp chicken yarp.i)
  set (YARP_COLLISION_AVOIDANCE TRUE)
ENDIF (CREATE_CHICKEN)



#############################################################################
## Create Allegro bindings

IF (CREATE_ALLEGRO)
  set( CMAKE_SWIG_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}" )
  set( ALLEGRO_INSTALL_PREFIX "/usr/local/lib/allegro/site" CACHE PATH "Install path prefix of allegro bindings." )
  set( ALLEGRO_ASDF_SYSTEMS "${ALLEGRO_INSTALL_PREFIX}/../asdf-systems" CACHE PATH "Path to register asdf file." )

  set( allegro_PROJ_FILE
    allegro/yarp.asd )

  set( allegro_SRC_FILE
    yarp.cl )

  set( allegro_LIBS
    yarp-allegro.so )

  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;-identifier-converter;lispify")
  swig_add_module(yarp-allegro allegrocl yarp.i)

  string( REGEX REPLACE "\\.cl" ".lisp" allegro_DEST_FILE ${allegro_SRC_FILE})
  install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/${allegro_PROJ_FILE} DESTINATION ${ALLEGRO_INSTALL_PREFIX}/yarp )
  install( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${allegro_SRC_FILE}
    DESTINATION ${ALLEGRO_INSTALL_PREFIX}/yarp/src
    RENAME ${allegro_DEST_FILE} )
  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/${allegro_LIBS} DESTINATION ${ALLEGRO_INSTALL_PREFIX}/yarp/lib/ )

  if( UNIX )
    install( CODE
      "execute_process( COMMAND \"mkdir\" \"-p\" \"${ALLEGRO_ASDF_SYSTEMS}\"
                              COMMAND \"ln\" \"-s\" \"${ALLEGRO_INSTALL_PREFIX}/yarp/yarp.asd\" \"${ALLEGRO_ASDF_SYSTEMS}\" )" )
  endif( UNIX )
  set (YARP_COLLISION_AVOIDANCE TRUE)
ENDIF (CREATE_ALLEGRO)

IF (CREATE_TCL)
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  FIND_PACKAGE(TCL REQUIRED)
  INCLUDE_DIRECTORIES(${TCL_INCLUDE_PATH})
  LINK_LIBRARIES(${TCL_LIBRARY})
  SWIG_ADD_MODULE(libtclyarp tcl yarp.i)
  set (YARP_COLLISION_AVOIDANCE TRUE)
ENDIF (CREATE_TCL)



#############################################################################
## Create C# bindings

IF (CREATE_CSHARP)

  # outdir does not seem to work?
  set (target_name yarp)
  if (YARP_COLLISION_AVOIDANCE)
    set (target_name yarp_cs)
  endif ()
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  IF (WIN32 AND NOT CYGWIN)
    OPTION(PREPARE_CLASS_FILES "Prepare NET library" ON)
    SWIG_ADD_MODULE(${target_name} csharp yarp.i)
    IF (PREPARE_CLASS_FILES)
      ADD_CUSTOM_COMMAND(TARGET ${target_name}
    POST_BUILD
    COMMAND IF NOT EXIST .\\generated_src mkdir generated_src
    WORKING_DIRECTORY ${YARP_BASE_DIR})

      ADD_CUSTOM_COMMAND(TARGET ${target_name}
    POST_BUILD
    COMMAND move *.cs .\\generated_src
    WORKING_DIRECTORY ${YARP_BASE_DIR})
    ENDIF ()
  ELSE(WIN32 AND NOT CYGWIN)
    SWIG_ADD_MODULE(${target_name} csharp yarp.i)
  ENDIF (WIN32 AND NOT CYGWIN)
  set (YARP_COLLISION_AVOIDANCE TRUE)
ENDIF (CREATE_CSHARP)



#############################################################################
## Create Ruby bindings

IF (CREATE_RUBY)
  FIND_PACKAGE(Ruby)
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  INCLUDE_DIRECTORIES(${RUBY_INCLUDE_PATH})
  set (target_name yarp)
  if (YARP_COLLISION_AVOIDANCE)
    set (target_name yarp_ruby)
  endif ()
  SWIG_ADD_MODULE(${target_name} ruby yarp.i)
  SWIG_LINK_LIBRARIES(${target_name} ${RUBY_LIBRARY})
  set_target_properties(${target_name} PROPERTIES PREFIX "")
  set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "yarp")
  set (YARP_COLLISION_AVOIDANCE TRUE)
ENDIF (CREATE_RUBY)




#############################################################################
## Create Lua bindings

IF (CREATE_LUA)
  SET(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  find_package(Lua51 REQUIRED) 
  set (target_name yarp_lua)
  INCLUDE_DIRECTORIES(${LUA_INCLUDE_DIR})
  SWIG_ADD_MODULE(${target_name} lua yarp.i)
  SWIG_LINK_LIBRARIES(${target_name} ${LUA_LIBRARY})
  set_target_properties(${target_name} PROPERTIES PREFIX "")
  set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "yarp")
  if (YARP_COLLISION_AVOIDANCE)
    set_target_properties(${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY 
      ${CMAKE_BINARY_DIR}/lib/lua)
  endif ()
  set (YARP_COLLISION_AVOIDANCE TRUE)
ENDIF (CREATE_LUA)

endif ()
