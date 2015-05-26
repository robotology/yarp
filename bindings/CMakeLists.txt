# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Arjan Gijsberts, Lorenzo Natale, Fabien Benureau, Stephane Lallee, Ali Paikan
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

cmake_minimum_required(VERSION 2.8.9)

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

if(YARP_COMPILE_BINDINGS)

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
    set(CREATE_${LANG} OFF CACHE BOOL "Do you want to create the ${lang} interface")
    if(CREATE_${LANG})
      message(STATUS " +++ bindings ${lang} is enabled")
    else()
      message(STATUS " +++ bindings ${lang} is not enabled")
    endif()
endforeach()


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
else()
  set(YARP_FOUND TRUE)
  get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
  get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
endif()
include_directories(${YARP_INCLUDE_DIRS})
link_libraries(${YARP_LIBRARIES})

set_source_files_properties(yarp.i PROPERTIES CPLUSPLUS ON)



#############################################################################
## Create Java bindings

if(CREATE_JAVA)
  option(PREPARE_CLASS_FILES "Prepare class files" OFF)
  set(YARP_HELPERS LoadYarp.java YarpImageHelper.java Example1.java)
  file(MAKE_DIRECTORY "${YARP_BASE_DIR}/yarp")
  set(CMAKE_SWIG_OUTDIR_SAVE ${CMAKE_SWIG_OUTDIR})
  set(CMAKE_SWIG_OUTDIR "${YARP_BASE_DIR}/yarp")
  set(TARGET_DIR src)
  if(NOT BINDINGS_SRC_DIR)
    set(BINDINGS_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
  endif()
  if(NOT PREPARE_CLASS_FILES)
    set(CMAKE_SWIG_OUTDIR "${YARP_BASE_DIR}/generated_src")
    set(TARGET_DIR generated_src)
  endif()
  file(MAKE_DIRECTORY "${YARP_BASE_DIR}/${TARGET_DIR}")
  foreach(src ${YARP_HELPERS})
    set(target ${YARP_BASE_DIR}/${TARGET_DIR}/${src})
    configure_file(${BINDINGS_SRC_DIR}/${src} ${target})
    message(STATUS "copied ${target}")
  endforeach(src)
  set(CMAKE_SWIG_FLAGS "-package;yarp;-Wall;-module;yarp")
  find_package(JNI REQUIRED)
  include_directories(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
  set(target_name jyarp)
  swig_add_module(${target_name} java yarp.i)
  if(PREPARE_CLASS_FILES)
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
  endif()

  # issue on MINGW where C++ name-mangling prevents java finding methods
  if(MINGW)
    message(STATUS "untested MINGW patch - see CMakeLists.txt")
    set_target_properties(${target_name} PROPERTIES
      LINK_FLAGS "--add-stdcall-alias")
  endif(MINGW)
  ## the following is based on:
  ## http://www.cmake.org/pipermail/cmake/2007-September/016343.html
  if(APPLE)
    set_target_properties(${target_name} PROPERTIES SUFFIX ".jnilib")
  endif(APPLE)
  set(YARP_COLLISION_AVOIDANCE TRUE)
  set(CMAKE_SWIG_OUTDIR ${CMAKE_SWIG_OUTDIR_SAVE})
endif(CREATE_JAVA)



#############################################################################
## Create Python bindings

if(CREATE_PYTHON)	 
  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;-threads")
  if(CREATE_PYTHON_VERSION) 
    message(WARNING "CREATE_PYTHON_VERSION was renamed YARP_USE_PYTHON_VERSION")
    set(YARP_USE_PYTHON_VERSION "${CREATE_PYTHON_VERSION}" CACHE STRING "Specify python version to use" )
    unset(CREATE_PYTHON_VERSION CACHE)
  else()
    set(YARP_USE_PYTHON_VERSION "" CACHE STRING "Specify python version to use" )
  endif()
  find_package(PythonInterp ${YARP_USE_PYTHON_VERSION} REQUIRED)
  set(YARP_USE_PYTHON_VERSION_ARGS)
  if(NOT YARP_USE_PYTHON_VERSION)
    set (YARP_USE_PYTHON_VERSION ${PYTHON_VERSION_STRING})
  endif()
  find_package(PythonLibs ${YARP_USE_PYTHON_VERSION} EXACT)
  include_directories(${PYTHON_INCLUDE_PATH})
  link_libraries(${PYTHON_LIBRARY})

  ## in Windows it seems necessary to declare explictly the link_directory
  if(WIN32)
    get_filename_component(PYTHON_DIR ${PYTHON_LIBRARY} PATH)
    link_directories(${PYTHON_DIR})
  endif()

  set(target_name yarp)
  if(YARP_COLLISION_AVOIDANCE)
    set(target_name yarp_python)
    set(CMAKE_SWIG_OUTDIR_SAVE ${CMAKE_SWIG_OUTDIR})
    set(CMAKE_SWIG_OUTDIR "${CMAKE_BINARY_DIR}/lib/python")
  endif()
  swig_add_module(${target_name} python yarp.i)
  # uncomment if we want a debug version of the library
  #add_definitions(-DPy_DEBUG)

  set_target_properties(_${target_name} PROPERTIES OUTPUT_NAME "_yarp")
  if(YARP_COLLISION_AVOIDANCE)
    set_target_properties(_${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/python")
  endif()

  # installation path is determined reliably on most platforms using distutils
  execute_process(COMMAND ${PYTHON_EXECUTABLE}
    -c "from distutils import sysconfig; print(sysconfig.get_python_lib(1,0,prefix=''))"
    OUTPUT_VARIABLE PYTHON_INSTDIR OUTPUT_STRIP_TRAILING_WHITESPACE )
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/yarp.py
    DESTINATION ${PYTHON_INSTDIR} )
  install(TARGETS _${target_name}
    DESTINATION ${PYTHON_INSTDIR} )

  if(WIN32)
    set_target_properties(_${target_name} PROPERTIES SUFFIX ".pyd")
  endif(WIN32)

  if(YARP_COLLISION_AVOIDANCE)
    set(CMAKE_SWIG_OUTDIR ${CMAKE_SWIG_OUTDIR_SAVE})
  else()
    set(YARP_COLLISION_AVOIDANCE TRUE)
  endif()
endif(CREATE_PYTHON)



#############################################################################
## Create Perl bindings

if(CREATE_PERL)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  find_package(PerlLibs)
  include_directories(${PERL_INCLUDE_PATH})
  swig_add_module(libperlyarp perl5 yarp.i)
  set(YARP_COLLISION_AVOIDANCE TRUE)
endif(CREATE_PERL)




#############################################################################
## Create Chicken bindings

if(CREATE_CHICKEN)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;cyarp")
  swig_add_module(libcyarp chicken yarp.i)
  set(YARP_COLLISION_AVOIDANCE TRUE)
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

  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp;-identifier-converter;lispify")
  swig_add_module(yarp-allegro allegrocl yarp.i)

  string(REGEX REPLACE "\\.cl" ".lisp" allegro_DEST_FILE ${allegro_SRC_FILE})
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${allegro_PROJ_FILE}
          DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${allegro_SRC_FILE}
          DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp/src
          RENAME ${allegro_DEST_FILE})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${allegro_LIBS}
          DESTINATION ${ALLEGRO_INSTALL_DIR}/yarp/lib)

  if(UNIX)
    install(CODE "execute_process(COMMAND \"mkdir\" \"-p\" \"${ALLEGRO_ASDF_SYSTEMS}\"
                                  COMMAND \"ln\" \"-s\" \"${CMAKE_INSTALL_PREFIX}/${ALLEGRO_INSTALL_DIR}/yarp/yarp.asd\" \"${CMAKE_INSTALL_PREFIX}/${ALLEGRO_ASDF_SYSTEMS}\")")
  endif()
endif()

if(CREATE_TCL)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  find_package(TCL REQUIRED)
  include_directories(${TCL_INCLUDE_PATH})
  link_libraries(${TCL_LIBRARY})
  swig_add_module(libtclyarp tcl yarp.i)
  set(YARP_COLLISION_AVOIDANCE TRUE)
endif(CREATE_TCL)



#############################################################################
## Create C# bindings

if(CREATE_CSHARP)

  # outdir does not seem to work?
  set(target_name yarp)
  if(YARP_COLLISION_AVOIDANCE)
    set(target_name yarp_cs)
  endif()

  if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.0)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
  endif()
  # -dllimport is actually needed only for CMake versions prior to 3.0 ( http://public.kitware.com/Bug/view.php?id=13814 )
  if(${CMAKE_VERSION} VERSION_LESS 3.0)
    set(CMAKE_SWIG_FLAGS "-Wall;-dllimport;${target_name}")
  else()
    set(CMAKE_SWIG_FLAGS "-Wall")
  endif()

  if(WIN32 AND NOT CYGWIN)
    option(PREPARE_CLASS_FILES "Prepare NET library" ON)
    swig_add_module(${target_name} csharp yarp.i)
    if(PREPARE_CLASS_FILES)
      add_custom_command(TARGET ${target_name}
                         POST_BUILD
                         COMMAND IF NOT EXIST .\\generated_src mkdir generated_src
                         WORKING_DIRECTORY ${YARP_BASE_DIR})

      add_custom_command(TARGET ${target_name}
                         POST_BUILD
                         COMMAND move *.cs ${YARP_BASE_DIR}/generated_src
                         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()
  else(WIN32 AND NOT CYGWIN)
    swig_add_module(${target_name} csharp yarp.i)
  endif(WIN32 AND NOT CYGWIN)
  set(YARP_COLLISION_AVOIDANCE TRUE)
endif(CREATE_CSHARP)



#############################################################################
## Create Ruby bindings

if(CREATE_RUBY)
  find_package(Ruby)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  include_directories(${RUBY_INCLUDE_PATH})
  set(target_name yarp)
  if(YARP_COLLISION_AVOIDANCE)
    set(target_name yarp_ruby)
  endif()
  swig_add_module(${target_name} ruby yarp.i)
  swig_link_libraries(${target_name} ${RUBY_LIBRARY})
  set_target_properties(${target_name} PROPERTIES PREFIX "")
  set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "yarp")
  set(YARP_COLLISION_AVOIDANCE TRUE)
endif(CREATE_RUBY)




#############################################################################
## Create Lua bindings

if(CREATE_LUA)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  find_package(Lua REQUIRED)
  set(target_name yarp_lua)
  include_directories(${LUA_INCLUDE_DIR})
  swig_add_module(${target_name} lua yarp.i)
  swig_link_libraries(${target_name} ${LUA_LIBRARY})
  set_target_properties(${target_name} PROPERTIES PREFIX "")
  set_target_properties(${target_name} PROPERTIES OUTPUT_NAME "yarp")
  if(YARP_COLLISION_AVOIDANCE)
    set_target_properties(${target_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/lua)
  endif()
  set(YARP_COLLISION_AVOIDANCE TRUE)

  # set(LUA_INSTALL_DIR "lib/lua/${LUA_VERSION_STRING}")
  # Unfortunatly LUA_VERSION_MAJOR and LUA_VERSION_MINOR are empty cmake (2.8.12.2)
  # So we extract them from LUA_VERSION_STRING

  string(REGEX MATCH "[0-9]*\\.[0-9]*" TMP_LUA_VERSION ${LUA_VERSION_STRING})
  set(LUA_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib/lua/${TMP_LUA_VERSION})

  # message(STATUS "--> ${LUA_INSTALL_DIR}")
  # message(STATUS ${LUA_INSTALL_DIR})
  install(TARGETS ${target_name}
    DESTINATION ${LUA_INSTALL_DIR})


endif(CREATE_LUA)

#############################################################################
## Create Octave bindings

if(CREATE_OCTAVE)
  set(CMAKE_SWIG_FLAGS "-Wall;-module;yarp")
  find_package(Octave REQUIRED)
  set(target_name yarp_octave)
  get_filename_component(OCTAVE_INCLUDE_TOP ${OCTAVE_INCLUDE_DIR} DIRECTORY)
  include_directories(${OCTAVE_INCLUDE_DIRS} ${OCTAVE_INCLUDE_TOP})
  swig_add_module(${target_name} octave yarp.i)
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
