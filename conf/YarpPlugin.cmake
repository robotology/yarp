# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#########################################################################
##
## This file provides a set of macros for building bundles of plugins.
## Sample use:
##
##  YARP_BEGIN_PLUGIN_LIBRARY(libname)
##    ADD_SUBDIRECTORY(plugin1)
##    ADD_SUBDIRECTORY(plugin2)
##    ...
##  YARP_END_PLUGIN_LIBRARY(libname)
##  YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE(libnamedev libname)
##
## This sample would create two CMake targets, "libname" (a library)
## and libnamedev (an executable).  It also defines a list:
##   ${libname_LIBRARIES}
## which contains a list of all library targets created within
## the plugin directories plugin1, plugin2, ...
##
## The "libname" library links with every library in the subdirectories
## (which can be made individually optional using CMake options),
## and provides a method to initialize them all.
##
## The executable is a test program that links and initializes
## the "libname" library, making the plugins accessible.
##
## Plugins come in two types, carriers and devices.
## To let YARP know how to initialize them, add lines like
## this in the CMakeLists.txt files the plugin subdirectories:
##
##   YARP_PREPARE_DEVICE(microphone TYPE MicrophoneDeviceDriver
##                  INCLUDE MicrophoneDeviceDriver.h WRAPPER grabber)
## (the WRAPPER is optional), or:
##   YARP_PREPARE_CARRIER(new_carrier TYPE TheCarrier INCLUDE ...)
##
#########################################################################


## Skip this whole file if it has already been included
if(COMMAND YARP_END_PLUGIN_LIBRARY)
  return()
endif()

include(GNUInstallDirs)
include(YarpInstallationHelpers)
include(CMakeParseArguments)


#########################################################################
# YARP_BEGIN_PLUGIN_LIBRARY: this macro makes sure that all the hooks
# needed for creating a plugin library are in place.  Between
# this call, and a subsequent call to END_PLUGIN_LIBRARY, the
# X_YARP_PLUGIN_MODE variable is set.  While this mode is set,
# any library targets created are tracked in a global list.
# Calls to this macro may be nested.
#
macro(YARP_BEGIN_PLUGIN_LIBRARY bundle_name)
  if(X_YARP_PLUGIN_MODE)

    # If we are nested inside a larger plugin block, we don't
    # have to do anything.
    message(STATUS "nested library ${bundle_name}")
  else()
    # If we are the outermost plugin block, then we need to set up
    # everything for tracking the plugins within that block.

    # Make a record of the fact that we are now within a plugin
    set(X_YARP_PLUGIN_MODE TRUE)

    # Declare that we are starting to compile the given plugin library
    message(STATUS "starting plugin library: ${bundle_name}")

    # Prepare a directory for automatically generated boilerplate code.
    set(X_YARP_PLUGIN_GEN ${CMAKE_BINARY_DIR}/generated_code)
    if(NOT EXISTS ${X_YARP_PLUGIN_GEN})
        file(MAKE_DIRECTORY ${X_YARP_PLUGIN_GEN})
    endif()

    # Choose a prefix for CMake options related to this library
    set(X_YARP_PLUGIN_PREFIX "${bundle_name}_")

    # Set a flag to let individual modules know that they are being
    # compiled as part of a bundle, and not standalone.  Developers
    # use this flag to inhibit compilation of test programs and
    # the like.
    set(COMPILE_PLUGIN_LIBRARY TRUE)
    set(COMPILE_DEVICE_LIBRARY TRUE) # an old name for the flag

    # Record the name of the plugin library name
    set(X_YARP_PLUGIN_MASTER ${bundle_name})

    # Set some properties to an empty state
    set_property(GLOBAL PROPERTY YARP_BUNDLE_PLUGINS) # list of plugins
    set_property(GLOBAL PROPERTY YARP_BUNDLE_OWNERS)  # owner library
    set_property(GLOBAL PROPERTY YARP_BUNDLE_LIBS)    # list of library targets
    set_property(GLOBAL PROPERTY YARP_BUNDLE_CODE)    # list of generated code
    set_property(GLOBAL PROPERTY YARP_BUNDLE_LINK_LIBRARIES) # list of additional libraries that will be linked by the master

    # One glitch is that if plugins are used within YARP, rather
    # than in an external library, then "find_package(YARP)" will
    # not work correctly yet.  We simulate the operation of
    # find_package(YARP) here if needed, using properties
    # maintained during the YARP build.
    get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
    if(YARP_TREE_INCLUDE_DIRS)
      # Simulate the operation of find_package(YARP)
      set(YARP_FOUND TRUE)
      get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
      get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
      get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
    else()
      find_package(YARP REQUIRED)
    endif()
  endif()
endmacro()



#########################################################################
# YARP_PREPARE_PLUGIN macro lets a developer declare a plugin using a
# statement like:
#    YARP_PREPARE_PLUGIN(foo CATEGORY device TYPE FooDriver INCLUDE FooDriver.h)
# or
#    YARP_PREPARE_PLUGIN(moto CATEGORY device TYPE Moto INCLUDE moto.h WRAPPER controlboard)
# This macro converts a plugin declaration to code, and to set up CMake
# flags for controlling compilation of that device.
#
macro(YARP_PREPARE_PLUGIN plugin_name)
  set(_options )
  set(_oneValueArgs TYPE INCLUDE WRAPPER CATEGORY)
  set(_multiValueArgs)
  cmake_parse_arguments(_YPP "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN} )

  if(NOT _YPP_TYPE OR NOT _YPP_INCLUDE)
    message(STATUS "Not enough information to create ${plugin_name}")
    message(STATUS "  type:     ${_YPP_TYPE}")
    message(STATUS "  include:  ${_YPP_INCLUDE}")
    message(STATUS "  wrapper:  ${_YPP_WRAPPER}")
    message(STATUS "  category: ${_YPP_CATEGORY}")
    return()
  endif()

  # Append the current source directory to the set of include paths.
  # Developers seem to expect #include "foo.h" to work if foo.h is
  # in their module directory.
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  # Figure out a decent filename for the code we are about to
  # generate.  If all else fails, the code will get dumped in
  # the current binary directory.
  set(fdir ${X_YARP_PLUGIN_GEN})
  if(NOT fdir)
    set(fdir ${CMAKE_CURRENT_BINARY_DIR})
  endif()

  # Set up a flag to enable/disable compilation of this plugin.
  set(X_MYNAME "${X_YARP_PLUGIN_PREFIX}${plugin_name}")
  if(NOT COMPILE_BY_DEFAULT)
    set(COMPILE_BY_DEFAULT FALSE)
  endif()
  set(ENABLE_${X_MYNAME} ${COMPILE_BY_DEFAULT} CACHE BOOL "Enable/disable compilation of ${X_MYNAME}")

  # Set some convenience variables based on whether the plugin
  # is enabled or disabled.
  set(ENABLE_${plugin_name} ${ENABLE_${X_MYNAME}})
  if(ENABLE_${plugin_name})
    set(SKIP_${plugin_name} FALSE)
    set(SKIP_${X_MYNAME} FALSE)
  else()
    set(SKIP_${plugin_name} TRUE)
    set(SKIP_${X_MYNAME} TRUE)
  endif()

  # If the plugin is enabled, add the appropriate source code into
  # the library source list.
  if(ENABLE_${X_MYNAME})
    # Go ahead and prepare some code to wrap this plugin.
    if(NOT "${X_YARP_PLUGIN_MASTER}" STREQUAL "")
      set(_fname ${fdir}/${X_YARP_PLUGIN_MASTER}_add_${plugin_name}.cpp)
    else()
      set(_fname ${fdir}/${_YPP_CATEGORY}_add_${plugin_name}.cpp)
    endif()

    # Variables used by the templates:
    set(YARPPLUG_NAME "${plugin_name}")
    set(YARPPLUG_TYPE "${_YPP_TYPE}")
    set(YARPPLUG_INCLUDE "${_YPP_INCLUDE}")
    set(YARPPLUG_WRAPPER "${_YPP_WRAPPER}")
    set(YARPPLUG_CATEGORY "${_YPP_CATEGORY}")
    configure_file(${YARP_MODULE_DIR}/template/yarp_plugin_${_YPP_CATEGORY}.cpp.in
                   ${_fname} @ONLY)

    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_PLUGINS ${plugin_name})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_CODE ${_fname})
    get_property(_link_libs GLOBAL PROPERTY YARP_BUNDLE_LINK_LIBRARIES)
    if("${_YPP_CATEGORY}" STREQUAL "carrier")
      list(APPEND _link_libs YARP::YARP_OS)
    elseif("${_YPP_CATEGORY}" STREQUAL "device")
      list(APPEND _link_libs YARP::YARP_OS YARP::YARP_dev)
    endif()
    if(NOT "${_link_libs}" STREQUAL "")
      list(REMOVE_DUPLICATES _link_libs)
    endif()
    set_property(GLOBAL PROPERTY YARP_BUNDLE_LINK_LIBRARIES ${_link_libs})
    message(STATUS " +++ plugin ${plugin_name}, ENABLE_${plugin_name} is set")
  else()
    message(STATUS " +++ plugin ${plugin_name}, SKIP_${plugin_name} is set")
  endif()
endmacro()



#########################################################################
# YARP_PREPARE_DEVICE macro lets a developer declare a device plugin using a
# statement like:
#    YARP_PREPARE_PLUGIN(moto CATEGORY device TYPE Moto INCLUDE moto.h WRAPPER controlboard)
#
macro(YARP_PREPARE_DEVICE)
  yarp_prepare_plugin(${ARGN} CATEGORY device)
endmacro()



#########################################################################
# YARP_PREPARE_CARRIER macro lets a developer declare a carrier plugin using a
# statement like:
#    YARP_PREPARE_CARRIER(foo TYPE FooCarrier INCLUDE FooCarrier.h)
#
macro(YARP_PREPARE_CARRIER)
  yarp_prepare_plugin(${ARGN} CATEGORY carrier)
endmacro()



#########################################################################
# YARP_ADD_PLUGIN macro tracks plugin libraries.  We want to
# be later able to link against them all as a group.
#
macro(YARP_ADD_PLUGIN LIBNAME)
  # YARP_ADD_PLUGIN used to be a re-definition of the ADD_LIBRARY
  # CMake command therefore IMPORTED libraries had to be skipped.
  # It should be safe to remove this check now, but for now issue a
  # fatal error to ensure that YARP_ADD_PLUGIN is not used instead
  # of ADD_LIBRARY
  # FIXME Remove this check as soon as we are sure that it is not used
  # anywhere
  foreach(arg ${ARGN})
    if("${arg}" STREQUAL "IMPORTED")
      message(FATAL_ERROR "YARP_ADD_PLUGIN does not support the IMPORTED argument.")
    endif()
  endforeach()

  if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
    set(X_LIBTYPE "MODULE")
  else()
    set(X_LIBTYPE "STATIC")
  endif()

  # The user is adding a bone-fide plugin library.  We add it,
  # while inserting any generated source code needed for initialization.
  get_property(srcs GLOBAL PROPERTY YARP_BUNDLE_CODE)
  foreach(s ${srcs})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_OWNERS ${LIBNAME})
  endforeach()
  get_property(_link_libs GLOBAL PROPERTY YARP_BUNDLE_LINK_LIBRARIES)
  if(TARGET YARP_OS)
    # Building YARP: Targets don't have NAMESPACE
    string(REPLACE "YARP::" "" _link_libs "${_link_libs}")
  endif()
  add_library(${LIBNAME} ${X_LIBTYPE} ${srcs} ${ARGN})
  target_link_libraries(${LIBNAME} ${_link_libs})
  if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
    # Do not add the "lib" prefix to dynamic plugin libraries.
    # This simplifies search on different platforms and makes it easier
    # to distinguish them from linkable libraries.
    set_property(TARGET ${LIBNAME} PROPERTY PREFIX "")
  else()
    # Compile static plugins with -DYARP_STATIC_PLUGIN
    set_property(TARGET ${LIBNAME} APPEND PROPERTY COMPILE_DEFINITIONS YARP_STATIC_PLUGIN)
  endif()

  # Add the library to the list of plugin libraries.
  set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_LIBS ${LIBNAME})
  # Reset the list of generated source code and link libraries to empty.
  set_property(GLOBAL PROPERTY YARP_BUNDLE_CODE)
  set_property(GLOBAL PROPERTY YARP_BUNDLE_LINK_LIBRARIES)
endmacro()



#########################################################################
# YARP_END_PLUGIN_LIBRARY macro finalizes a plugin library if this is
# the outermost plugin library block, otherwise it propagates
# all collected information to the plugin library block that wraps
# it.
#
macro(YARP_END_PLUGIN_LIBRARY bundle_name)
  # make sure we are the outermost plugin library, if nesting is present.
  if(NOT "${bundle_name}" STREQUAL "${X_YARP_PLUGIN_MASTER}")
    # If we are nested inside a larger plugin block, we don't
    # have to do anything.
    message(STATUS "ending nested plugin library ${bundle_name}")
  else()
    message(STATUS "ending plugin library: ${bundle_name}")
    # generate code to call all plugin initializers
    set(YARP_LIB_NAME ${X_YARP_PLUGIN_MASTER})
    get_property(devs GLOBAL PROPERTY YARP_BUNDLE_PLUGINS)
    get_property(owners GLOBAL PROPERTY YARP_BUNDLE_OWNERS)
    set(YARP_CODE_PRE)
    set(YARP_CODE_POST)
    if(NOT YARP_FORCE_DYNAMIC_PLUGINS AND NOT BUILD_SHARED_LIBS)
      foreach(dev ${devs})
        if(NOT owners)
          message(SEND_ERROR "No owner for device ${dev}, this is likely due to a previous error, check the output of CMake above.")
        endif()
        list(GET owners 0 owner)
        list(REMOVE_AT owners 0)
        set(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern YARP_PLUGIN_IMPORT void add_owned_${dev}(const char *str);")
        set(YARP_CODE_POST "${YARP_CODE_POST}\n        add_owned_${dev}(\"${owner}\");")
      endforeach()
    endif()
    configure_file(${YARP_MODULE_DIR}/template/yarp_plugin_lib.cpp.in
                   ${X_YARP_PLUGIN_GEN}/add_${X_YARP_PLUGIN_MASTER}_plugins.cpp @ONLY)
    get_property(code GLOBAL PROPERTY YARP_BUNDLE_CODE)
    include_directories(${YARP_INCLUDE_DIRS})
    get_property(libs GLOBAL PROPERTY YARP_BUNDLE_LIBS)
    # add the library initializer code
    add_library(${X_YARP_PLUGIN_MASTER} ${code} ${X_YARP_PLUGIN_GEN}/add_${X_YARP_PLUGIN_MASTER}_plugins.cpp)

    if(NOT YARP_FORCE_DYNAMIC_PLUGINS AND NOT BUILD_SHARED_LIBS)
      set_property(TARGET ${X_YARP_PLUGIN_MASTER} APPEND PROPERTY COMPILE_DEFINITIONS YARP_STATIC_PLUGIN)
      target_link_libraries(${X_YARP_PLUGIN_MASTER} LINK_PRIVATE ${libs})
    endif()
    # give user access to a list of all the plugin libraries
    set(${X_YARP_PLUGIN_MASTER}_LIBRARIES ${libs})
    set(X_YARP_PLUGIN_MODE FALSE) # neutralize redefined methods
  endif()
endmacro()



#########################################################################
# YARP_ADD_PLUGIN_YARPDEV_EXECUTABLE macro expands yarpdev executable
# for a named plugin library.
#
macro(YARP_ADD_PLUGIN_YARPDEV_EXECUTABLE exename bundle_name)
  if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
    set(YARP_CODE_PRE)
    set(YARP_CODE_POST)
  else()
    set(YARP_CODE_PRE "YARP_DECLARE_DEVICES(${bundle_name})")
    set(YARP_CODE_POST "    YARP_REGISTER_DEVICES(${bundle_name})")
  endif()
  configure_file(${YARP_MODULE_DIR}/template/yarp_plugin_yarpdev_main.cpp.in
                  ${X_YARP_PLUGIN_GEN}/${bundle_name}_yarpdev.cpp @ONLY)
  add_executable(${exename} ${X_YARP_PLUGIN_GEN}/${bundle_name}_yarpdev.cpp)
  target_link_libraries(${exename} ${bundle_name})
  if(TARGET YARP_OS)
    # Building YARP
    target_link_libraries(${exename} YARP_OS YARP_init YARP_dev)
  else()
    target_link_libraries(${exename} YARP::YARP_OS YARP::YARP_init YARP::YARP_dev)
  endif()
endmacro()



#########################################################################
# Deprecated macros
#
if(NOT YARP_NO_DEPRECATED)
  include(${CMAKE_CURRENT_LIST_DIR}/YarpDeprecatedWarning.cmake)

  macro(BEGIN_PLUGIN_LIBRARY)
    yarp_deprecated_warning("BEGIN_PLUGIN_LIBRARY is deprecated. Use YARP_BEGIN_PLUGIN_LIBRARY instead.")
    yarp_begin_plugin_library(${ARGN})
  endmacro()

  macro(ADD_PLUGIN_NORMALIZED)
    yarp_deprecated_warning("ADD_PLUGIN_NORMALIZED is deprecated. Use YARP_ADD_PLUGIN_NORMALIZED instead.")
    yarp_add_plugin_normalized(${ARGN})
  endmacro()

  macro(PREPARE_PLUGIN)
    yarp_deprecated_warning("PREPARE_PLUGIN is deprecated. Use YARP_PREPARE_PLUGIN instead.")
    yarp_prepare_plugin(${ARGN})
  endmacro()

  macro(PREPARE_DEVICE)
    yarp_deprecated_warning("PREPARE_DEVICE is deprecated. Use YARP_PREPARE_DEVICE instead.")
    yarp_prepare_device(${ARGN})
  endmacro()

  macro(PREPARE_CARRIER)
    yarp_deprecated_warning("PREPARE_CARRIER is deprecated. Use YARP_PREPARE_CARRIER instead.")
    yarp_prepare_carrier(${ARGN})
  endmacro()

  macro(END_PLUGIN_LIBRARY)
    yarp_deprecated_warning("END_PLUGIN_LIBRARY is deprecated. Use YARP_END_PLUGIN_LIBRARY instead.")
    yarp_end_plugin_library(${ARGN})
  endmacro()

  macro(ADD_PLUGIN_LIBRARY_EXECUTABLE)
    yarp_deprecated_warning("ADD_PLUGIN_LIBRARY_EXECUTABLE is deprecated. Use YARP_ADD_PLUGIN_YARPDEV_EXECUTABLE instead.")
    yarp_add_plugin_yarpdev_executable(${ARGN})
  endmacro()

  macro(YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE)
    yarp_deprecated_warning("YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE is deprecated. Use YARP_ADD_PLUGIN_YARPDEV_EXECUTABLE instead.")
    yarp_add_plugin_yarpdev_executable(${ARGN})
  endmacro()

  macro(YARP_ADD_CARRIER_FINGERPRINT file_name)
    yarp_deprecated_warning("YARP_ADD_CARRIER_FINGERPRINT is deprecated. Use YARP_INSTALL instead.")
    yarp_install(FILES ${file_name}
                 COMPONENT runtime
                 DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  endmacro()

  macro(YARP_ADD_DEVICE_FINGERPRINT file_name)
    yarp_deprecated_warning("YARP_ADD_DEVICE_FINGERPRINT is deprecated. Use YARP_INSTALL instead.")
    yarp_install(FILES ${file_name}
                 COMPONENT runtime
                 DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  endmacro()
endif()



#########################################################################
# Lightly redefine FIND_PACKAGE to skip calls to FIND_PACKAGE(YARP).
# YARP dependencies are guaranteed to have already been satisfied.
# And if we are compiling YARP, the use of FIND_PACKAGE(YARP) will lead
# to problems.
# FIXME This macro must disappear.
#
macro(FIND_PACKAGE LIBNAME)
  if(NOT X_YARP_PLUGIN_MODE)
    # pass on call without looking at it
    _FIND_PACKAGE(${LIBNAME} ${ARGN})
  endif()
  if(NOT "${LIBNAME}" STREQUAL "YARP")
    # Skipping requests for YARP, we already have it
    _FIND_PACKAGE(${LIBNAME} ${ARGN})
  endif()
endmacro()
