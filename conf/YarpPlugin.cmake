# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#########################################################################
##
## This file provides a set of macros for building bundles of plugins.
## Sample use:
##
##  BEGIN_PLUGIN_LIBRARY(libname)
##    ADD_SUBDIRECTORY(plugin1)
##    ADD_SUBDIRECTORY(plugin2)
##    ...
##  END_PLUGIN_LIBRARY(libname)
##  ADD_PLUGIN_LIBRARY_EXECUTABLE(libnamedev libname)
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
##   PREPARE_DEVICE(microphone TYPE MicrophoneDeviceDriver 
##                  INCLUDE MicrophoneDeviceDriver.h WRAPPER grabber)
## (the WRAPPER is optional), or:
##   PREPARE_CARRIER(new_carrier TYPE TheCarrier INCLUDE ...)
##
#########################################################################


## Skip this whole file if it has already been included
IF (NOT COMMAND END_PLUGIN_LIBRARY)


#########################################################################
# BEGIN_PLUGIN_LIBRARY: this macro makes sure that all the hooks
# needed for creating a plugin library are in place.  Between
# this call, and a subsequent call to END_PLUGIN_LIBRARY, the
# YARP_PLUGIN_MODE variable is set.  While this mode is set,
# any library targets created are tracked in a global list.
# Calls to this macro may be nested.
#
MACRO(BEGIN_PLUGIN_LIBRARY bundle_name)

  IF (YARP_PLUGIN_MODE)

    # If we are nested inside a larger plugin block, we don't
    # have to do anything.
    MESSAGE(STATUS "nested library ${bundle_name}")

  ELSE (YARP_PLUGIN_MODE)

    # If we are the outermost plugin block, then we need to set up 
    # everything for tracking the plugins within that block.

    # Make a record of the fact that we are now within a plugin
    SET(YARP_PLUGIN_MODE TRUE)

    # Declare that we are starting to compile the given plugin library
    MESSAGE(STATUS "starting plugin library: ${bundle_name}")

    # Prepare a directory for automatically generated boilerplate code.
    SET(YARP_PLUGIN_GEN ${CMAKE_BINARY_DIR}/generated_code)
    IF (NOT EXISTS ${YARP_PLUGIN_GEN})
      FILE(MAKE_DIRECTORY ${YARP_PLUGIN_GEN})
    ENDIF (NOT EXISTS ${YARP_PLUGIN_GEN})

    # Choose a prefix for CMake options related to this library
    SET(YARP_PLUGIN_PREFIX "${bundle_name}_")

    # Set a flag to let individual modules know that they are being
    # compiled as part of a bundle, and not standalone.  Developers
    # use this flag to inhibit compilation of test programs and 
    # the like.
    SET(COMPILE_PLUGIN_LIBRARY TRUE)
    SET(COMPILE_DEVICE_LIBRARY TRUE) # an old name for the flag

    # Record the name of the plugin library name
    SET(YARP_PLUGIN_MASTER ${bundle_name})

    # Set some properties to an empty state
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_PLUGINS) # list of plugins
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_STUBS)   # list of stubs
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_STUB_CODE) # initializers
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_OWNERS)   # owner library
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_LIBS)    # list of library targets
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_LINKS)   # list of link directories
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_CODE)    # list of generated code
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_RUNTIME) # is library lazy?

    # One glitch is that if plugins are used within YARP, rather
    # than in an external library, then "find_package(YARP)" will
    # not work correctly yet.  We simulate the operation of 
    # find_package(YARP) here if needed, using properties
    # maintained during the YARP build.
    get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
    if (YARP_TREE_INCLUDE_DIRS)
      # Simulate the operation of find_package(YARP)
      set (YARP_FOUND TRUE)
      get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
      get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
      get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
    else (YARP_TREE_INCLUDE_DIRS)
      find_package(YARP REQUIRED)
    endif (YARP_TREE_INCLUDE_DIRS)

  ENDIF (YARP_PLUGIN_MODE)

ENDMACRO(BEGIN_PLUGIN_LIBRARY bundle_name)


#########################################################################
# ADD_PLUGIN_NORMALIZED macro is an internal command to convert a 
# plugin declaration to code, and to set up CMake flags for controlling
# compilation of that device.  This macro is called by PREPARE_PLUGIN
# which is the user-facing macro.  PREPARE_PLUGIN parses
# a flexible set of arguments, then passes them to ADD_PLUGIN_NORMALIZED
# in a clean canonical order.
#
MACRO(ADD_PLUGIN_NORMALIZED plugin_name type include wrapper category)

  # Append the current source directory to the set of include paths.
  # Developers seem to expect #include "foo.h" to work if foo.h is
  # in their module directory.
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  # Figure out a decent filename for the code we are about to 
  # generate.  If all else fails, the code will get dumped in
  # the current binary directory.
  SET(fdir ${YARP_PLUGIN_GEN})
  IF(NOT fdir)
    SET(fdir ${CMAKE_CURRENT_BINARY_DIR})
  ENDIF(NOT fdir)

  # We'll be expanding the code in template/yarp_plugin_*.cpp.in using 
  # the following variables:

  SET(YARPDEV_NAME "${plugin_name}")
  SET(YARPDEV_TYPE "${type}")
  SET(YARPDEV_INCLUDE "${include}")
  SET(YARPDEV_WRAPPER "${wrapper}")
  SET(YARPDEV_CATEGORY "${category}")
  #SET(ENABLE_YARPDEV_NAME "1")

  # Set up a flag to enable/disable compilation of this plugin.
  SET(MYNAME "${YARP_PLUGIN_PREFIX}${plugin_name}")
  IF (NOT COMPILE_BY_DEFAULT)
    SET (COMPILE_BY_DEFAULT FALSE)
  ENDIF (NOT COMPILE_BY_DEFAULT)
  SET(ENABLE_${MYNAME} ${COMPILE_BY_DEFAULT} CACHE BOOL "Enable/disable compilation of ${MYNAME}")
  IF (ENABLE_${MYNAME})
    SET(RUNTIME_${MYNAME} ${COMPILE_BY_DEFAULT} CACHE BOOL "Enable/disable loading of ${MYNAME} at runtime upon need")
    # MARK_AS_ADVANCED(CLEAR RUNTIME_${MYNAME}) # don't advertise, for now
    MARK_AS_ADVANCED(FORCE RUNTIME_${MYNAME})
  ELSE (ENABLE_${MYNAME})
    MARK_AS_ADVANCED(FORCE RUNTIME_${MYNAME})
  ENDIF (ENABLE_${MYNAME})

  # Set some convenience variables based on whether the plugin
  # is enabled or disabled.
  SET(ENABLE_${plugin_name} ${ENABLE_${MYNAME}})
  IF (ENABLE_${plugin_name})
    SET(SKIP_${plugin_name} FALSE)
    SET(SKIP_${MYNAME} FALSE)
  ELSE (ENABLE_${plugin_name})
    SET(SKIP_${plugin_name} TRUE)
    SET(SKIP_${MYNAME} TRUE)
  ENDIF (ENABLE_${plugin_name})

  # If the plugin is enabled, add the appropriate source code into
  # the library source list.
  IF (ENABLE_${MYNAME})
    # Go ahead and prepare some code to wrap this plugin.
    SET(fname ${fdir}/yarpdev_add_${plugin_name}.cpp)
    SET(fname_stub ${fdir}/yarpdev_stub_${plugin_name}.cpp)
    SET(RUNTIME_YARPDEV ${RUNTIME_${MYNAME}})
    CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarp_plugin_${category}.cpp.in
      ${fname} @ONLY  IMMEDIATE)
 
    IF (RUNTIME_${MYNAME})
      set_property(GLOBAL PROPERTY YARP_BUNDLE_RUNTIME TRUE)
      CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarp_stub_${category}.cpp.in
	${fname_stub} @ONLY IMMEDIATE)
      set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_STUBS ${plugin_name})
      set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_STUB_CODE ${fname_stub})
    ENDIF (RUNTIME_${MYNAME})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_PLUGINS ${plugin_name})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_CODE ${fname})
    SET(YARP_PLUGIN_ACTIVE TRUE)
    MESSAGE(STATUS " +++ plugin ${plugin_name}, ENABLE_${plugin_name} is set")
  ELSE (ENABLE_${MYNAME})
    MESSAGE(STATUS " +++ plugin ${plugin_name}, SKIP_${plugin_name} is set")
  ENDIF (ENABLE_${MYNAME})

  # We are done!

ENDMACRO(ADD_PLUGIN_NORMALIZED plugin_name type include wrapper)



#########################################################################
# PREPARE_PLUGIN macro lets a developer declare a plugin using a 
# statement like:
#    PREPARE_PLUGIN(foo CATEGORY device TYPE FooDriver INCLUDE FooDriver.h)
# or
#    PREPARE_PLUGIN(moto CATEGORY device TYPE Moto INCLUDE moto.h WRAPPER controlboard)
# This macro is just a simple parser and calls ADD_PLUGIN_NORMALIZED to
# do the actual work.
#
MACRO(PREPARE_PLUGIN plugin_name)
  SET(EXPECT_TYPE FALSE)
  SET(EXPECT_INCLUDE FALSE)
  SET(THE_TYPE "")
  SET(THE_INCLUDE "")
  SET(THE_WRAPPER "")
  FOREACH(arg ${ARGN})
    IF(EXPECT_TYPE)
      SET(THE_TYPE ${arg})
      SET(EXPECT_TYPE FALSE)
    ENDIF(EXPECT_TYPE)
    IF(EXPECT_INCLUDE)
      SET(THE_INCLUDE ${arg})
      SET(EXPECT_INCLUDE FALSE)
    ENDIF(EXPECT_INCLUDE)
    IF(EXPECT_WRAPPER)
      SET(THE_WRAPPER ${arg})
      SET(EXPECT_WRAPPER FALSE)
    ENDIF(EXPECT_WRAPPER)
    IF(EXPECT_CATEGORY)
      SET(THE_CATEGORY ${arg})
      SET(EXPECT_CATEGORY FALSE)
    ENDIF(EXPECT_CATEGORY)
    IF(arg STREQUAL "TYPE")
      SET(EXPECT_TYPE TRUE)
    ENDIF(arg STREQUAL "TYPE")
    IF(arg STREQUAL "INCLUDE")
      SET(EXPECT_INCLUDE TRUE)
    ENDIF(arg STREQUAL "INCLUDE")
    IF(arg STREQUAL "WRAPPER")
      SET(EXPECT_WRAPPER TRUE)
    ENDIF(arg STREQUAL "WRAPPER")
    IF(arg STREQUAL "CATEGORY")
      SET(EXPECT_CATEGORY TRUE)
    ENDIF(arg STREQUAL "CATEGORY")
  ENDFOREACH(arg ${ARGN})
  IF(THE_TYPE AND THE_INCLUDE)
    ADD_PLUGIN_NORMALIZED(${plugin_name} ${THE_TYPE} ${THE_INCLUDE} "${THE_WRAPPER}" "${THE_CATEGORY}")
  ELSE(THE_TYPE AND THE_INCLUDE)
    MESSAGE(STATUS "Not enough information to create ${plugin_name}")
    MESSAGE(STATUS "  type:    ${THE_TYPE}")
    MESSAGE(STATUS "  include: ${THE_INCLUDE}")
    MESSAGE(STATUS "  wrapper: ${THE_WRAPPER}")
    MESSAGE(STATUS "  category: ${THE_CATEGORY}")
  ENDIF(THE_TYPE AND THE_INCLUDE)
ENDMACRO(PREPARE_PLUGIN plugin_name)



#########################################################################
# PREPARE_DEVICE macro lets a developer declare a device plugin using a 
# statement like:
#    PREPARE_PLUGIN(moto CATEGORY device TYPE Moto INCLUDE moto.h WRAPPER controlboard)
#
MACRO(PREPARE_DEVICE)
  PREPARE_PLUGIN(${ARGN} CATEGORY device)
ENDMACRO(PREPARE_DEVICE)



#########################################################################
# PREPARE_CARRIER macro lets a developer declare a carrier plugin using a 
# statement like:
#    PREPARE_CARRIER(foo TYPE FooCarrier INCLUDE FooCarrier.h)
#
MACRO(PREPARE_CARRIER)
  PREPARE_PLUGIN(${ARGN} CATEGORY carrier)
ENDMACRO(PREPARE_CARRIER)



#########################################################################
# Lightly redefine ADD_LIBRARY to track plugin libraries.  We want to
# be later able to link against them all as a group.
#
MACRO(ADD_LIBRARY LIBNAME)
  IF (NOT YARP_PLUGIN_MODE)
    # when not compiling a plugin library, revert to normal operation
    _ADD_LIBRARY(${LIBNAME} ${ARGN})
  ELSE (NOT YARP_PLUGIN_MODE)
    # we check to see if the ADD_LIBRARY call is an import, and ignore
    # if so - we don't need to do anything about imports.
    set(IS_IMPORTED FALSE)
    foreach(arg ${ARGN})
      if ("${arg}" STREQUAL "IMPORTED")
        set(IS_IMPORTED TRUE)
      endif()
    endforeach()
    if (NOT IS_IMPORTED)
      # The user is adding a bone-fide plugin library.  We add it,
      # while inserting any generated source code needed for initialization.
      get_property(srcs GLOBAL PROPERTY YARP_BUNDLE_CODE)
      foreach(s ${srcs})
	set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_OWNERS ${LIBNAME})
      endforeach()
      _ADD_LIBRARY(${LIBNAME} ${srcs} ${ARGN})
      # Reset the list of generated source code to empty.
      get_property(YARP_BUNDLE_RUNTIME GLOBAL PROPERTY YARP_BUNDLE_RUNTIME)
      if (NOT YARP_BUNDLE_RUNTIME)
	# Add the library to the list of plugin libraries.
	set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_LIBS ${LIBNAME})
      endif()
      set_property(GLOBAL PROPERTY YARP_BUNDLE_CODE)
      set_property(GLOBAL PROPERTY YARP_BUNDLE_RUNTIME)
      if (YARP_TREE_INCLUDE_DIRS)
        # If compiling YARP, we go ahead and set up installing this
        # target.  It isn't safe to do this outside of YARP though.
        install(TARGETS ${LIBNAME} EXPORT YARP COMPONENT runtime 
                DESTINATION lib)
      endif (YARP_TREE_INCLUDE_DIRS)
    endif (NOT IS_IMPORTED)
  ENDIF (NOT YARP_PLUGIN_MODE)
ENDMACRO(ADD_LIBRARY LIBNAME)



#########################################################################
# Lightly redefine LINK_DIRECTORIES to track link path changes.  Rarely
# needed.
#
MACRO(LINK_DIRECTORIES)
  _LINK_DIRECTORIES(${ARGN})
  IF (YARP_PLUGIN_MODE)
    # Add to the list of linked directories.
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_LINKS ${ARGN})
    set_property(GLOBAL APPEND PROPERTY YARP_TREE_LINK_DIRS ${ARGN})
  ENDIF (YARP_PLUGIN_MODE)
ENDMACRO(LINK_DIRECTORIES)



#########################################################################
# Lightly redefine FIND_PACKAGE to skip calls to FIND_PACKAGE(YARP).
# YARP dependencies are guaranteed to have already been satisfied.
# And if we are compiling YARP, the use of FIND_PACKAGE(YARP) will lead
# to problems.
#
MACRO(FIND_PACKAGE LIBNAME)
  IF (NOT YARP_PLUGIN_MODE)
    # pass on call without looking at it
    _FIND_PACKAGE(${LIBNAME} ${ARGN})
  ELSE (NOT YARP_PLUGIN_MODE)
  ENDIF (NOT YARP_PLUGIN_MODE)
    IF ("${LIBNAME}" STREQUAL "YARP")
      # Skipping requests for YARP, we already have it
    ELSE ("${LIBNAME}" STREQUAL "YARP")
      _FIND_PACKAGE(${LIBNAME} ${ARGN})
    ENDIF ("${LIBNAME}" STREQUAL "YARP")
ENDMACRO(FIND_PACKAGE LIBNAME)



#########################################################################
# END_PLUGIN_LIBRARY macro finalizes a plugin library if this is
# the outermost plugin library block, otherwise it propagates
# all collected information to the plugin library block that wraps
# it.
#
macro(END_PLUGIN_LIBRARY bundle_name)
  message(STATUS "ending plugin library: ${bundle_name}")
  # make sure we are the outermost plugin library, if nesting is present.
  if ("${bundle_name}" STREQUAL "${YARP_PLUGIN_MASTER}")
    # generate code to call all plugin initializers
    set(YARP_LIB_NAME ${YARP_PLUGIN_MASTER})
    get_property(devs GLOBAL PROPERTY YARP_BUNDLE_PLUGINS)
    get_property(owners GLOBAL PROPERTY YARP_BUNDLE_OWNERS)
    set(YARP_CODE_PRE)
    set(YARP_CODE_POST)
    foreach(dev ${devs})
      if(NOT owners)
	    message(SEND_ERROR "Empty list (owner), this is likely due to a previous error, check the output of CMake above.")
	  endif()
	  list(GET owners 0 owner)
      list(REMOVE_AT owners 0)
      set(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern YARP_PLUGIN_IMPORT void add_owned_${dev}(const char *str);")
      set(YARP_CODE_POST "${YARP_CODE_POST}\n    add_owned_${dev}(\"${owner}\");")
    endforeach()
    configure_file(${YARP_MODULE_PATH}/template/yarpdev_lib.cpp.in
      ${YARP_PLUGIN_GEN}/add_${YARP_PLUGIN_MASTER}_plugins.cpp @ONLY IMMEDIATE)
    configure_file(${YARP_MODULE_PATH}/template/yarpdev_lib.h.in
      ${YARP_PLUGIN_GEN}/add_${YARP_PLUGIN_MASTER}_plugins.h @ONLY  IMMEDIATE)
    get_property(code GLOBAL PROPERTY YARP_BUNDLE_CODE)
    get_property(code_stub GLOBAL PROPERTY YARP_BUNDLE_STUB_CODE)
    include_directories(${YARP_INCLUDE_DIRS})
    get_property(libs GLOBAL PROPERTY YARP_BUNDLE_LIBS)
    get_property(links GLOBAL PROPERTY YARP_BUNDLE_LINKS)
    if (links)
      _link_directories(${links})
    endif ()
    # add the library initializer code
    _ADD_LIBRARY(${YARP_PLUGIN_MASTER} ${code} ${code_stub} ${YARP_PLUGIN_GEN}/add_${YARP_PLUGIN_MASTER}_plugins.cpp)
    target_link_libraries(${YARP_PLUGIN_MASTER} ${YARP_LIBRARIES})
    target_link_libraries(${YARP_PLUGIN_MASTER} ${libs})
    # give user access to a list of all the plugin libraries
    set(${YARP_PLUGIN_MASTER}_LIBRARIES ${libs})
    set(YARP_PLUGIN_MODE FALSE) # neutralize redefined methods 
  endif ("${bundle_name}" STREQUAL "${YARP_PLUGIN_MASTER}")
endmacro(END_PLUGIN_LIBRARY bundle_name)



#########################################################################
# ADD_PLUGIN_LIBRARY_EXECUTABLE macro expands a simple test program
# for a named device library.
#
MACRO(ADD_PLUGIN_LIBRARY_EXECUTABLE exename bundle_name)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib_main.cpp.in
    ${YARP_PLUGIN_GEN}/yarpdev_${bundle_name}.cpp @ONLY  IMMEDIATE)
    ADD_EXECUTABLE(${exename} ${YARP_PLUGIN_GEN}/yarpdev_${bundle_name}.cpp)
    TARGET_LINK_LIBRARIES(${exename} ${bundle_name})
ENDMACRO(ADD_PLUGIN_LIBRARY_EXECUTABLE)



#########################################################################
# YARP_ADD_CARRIER_FINGERPRINT macro gives YARP a config file that will help
# detect that a message is in a particular protocol, even if support for
# that protocol has not yet loaded:
#    YARP_ADD_CARRIER_FINGERPRINT(carrier.ini carrier1 carrier2)
#
macro(YARP_ADD_CARRIER_FINGERPRINT file_name)
  GET_FILENAME_COMPONENT(out_name ${file_name} NAME)
  CONFIGURE_FILE(${file_name} ${CMAKE_BINARY_DIR}/etc/yarp/plugins/${out_name} COPYONLY)
  if (YARP_TREE_INCLUDE_DIRS)
    install(FILES ${CMAKE_BINARY_DIR}/etc/yarp/plugins/${out_name} COMPONENT runtime DESTINATION etc/yarp/plugins)
  endif (YARP_TREE_INCLUDE_DIRS)
endmacro(YARP_ADD_CARRIER_FINGERPRINT)


#########################################################################
# YARP_ADD_DEVICE_FINGERPRINT macro gives YARP a config file that will help
# give information about a device that has not yet loaded
#    YARP_ADD_DEVICE_FINGERPRINT(device.ini device1 device2)
#
macro(YARP_ADD_DEVICE_FINGERPRINT)
  # no difference between fingerprint macros
  YARP_ADD_CARRIER_FINGERPRINT(${ARGN})
endmacro(YARP_ADD_DEVICE_FINGERPRINT)



## We skipped this whole file if it was already included
ENDIF (NOT COMMAND END_PLUGIN_LIBRARY)


