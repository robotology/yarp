# Copyright: (C) 2009, 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

#########################################################################
##
## Yet Another Device Compiling Monster, But Smaller This Time
##
## This file provides a set of macros for building bundles of devices.
##  BEGIN_DEVICE_LIBRARY(libname)
##  END_DEVICE_LIBRARY(libname)
## etc.
##
## Any ADD_LIBRARY commands that lie between BEGIN_DEVICE_LIBRARY and 
## END_DEVICE_LIBRARY will be intercepted, in order to track the 
## list of libraries created.  There are all linked together by
## a single library with the specified name.
##
## Some extra generated files will be included as well in the library
## in order to facilitate access to devices.
##
## For each device <devname> that gets compiled, there will be a:
##   yarpdev_add_<devname>.cpp
## file that contains a method called:
##   extern "C" void add_<devname>();
## which adds that device into the YARP list of device factories.
## For each device bundle/library there will be a:
##   add_<libname>.cpp
## with a method called:
##   extern "C" void add_<libname>_devices();
## which calls all the add_*() methods for devices within that bundle.
##


IF (COMMAND END_DEVICE_LIBRARY)
  MESSAGE(STATUS "Skipping YarpDevice.cmake, already included")
ELSE (COMMAND END_DEVICE_LIBRARY)


#########################################################################
# BEGIN_DEVICE_LIBRARY macro makes sure that all the hooks
# needed for creating a device library are in place.
#
MACRO(BEGIN_DEVICE_LIBRARY devname)

  # If we are nested inside a larger device block, we don't
  # have to do much.  If we are the outermost device block,
  # then we need to set up everything.
  IF (YARPY_DEVICES)

    MESSAGE(STATUS "nested library ${devname}")

  ELSE (YARPY_DEVICES)

    # Declare that we are starting to compile the given device bundle
    MESSAGE(STATUS "starting device library: ${devname}")
    SET(YARPY_DEV_LIB_NAME ${devname})
    SET(YARPY_DEVICES TRUE)

    SET(YARPY_DEV_GEN ${CMAKE_BINARY_DIR}/generated_code)
    IF (NOT EXISTS ${YARPY_DEV_GEN})
      FILE(MAKE_DIRECTORY ${YARPY_DEV_GEN})
    ENDIF (NOT EXISTS ${YARPY_DEV_GEN})
    SET(DEVICE_PREFIX "${devname}_")
    SET(YARPY_LIB_FLAG EXCLUDE_FROM_ALL)
    IF (NOT YARPY_DEV_LIB_NAME)
      SET(YARPY_DEV_LIB_NAME devices)
    ENDIF (NOT YARPY_DEV_LIB_NAME)

    # Check if hooks have been added
    IF (COMMAND END_DEVICE_LIBRARY)
      SET(YARPY_DEVICES_INSTALLED TRUE)
    ENDIF (COMMAND END_DEVICE_LIBRARY)

    # Set a flag to let individual modules know that they are being
    # compiled as part of a bundle, and not standalone.  Developers
    # use this flag to inhibit compilation of test programs and 
    # the like.
    SET(COMPILE_DEVICE_LIBRARY TRUE)

    # Record the name of this outermost device bundle (needed because
    # nesting is allowed)
    SET(YARPY_MASTER_DEVICE ${devname})

    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_DEVICES)
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_LIBS)
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_CODE)
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_INCLUDE_DIRS)

    get_property(YARP_TREE_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
    if (YARP_TREE_INCLUDE_DIRS)
      set (YARP_FOUND TRUE)
      get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
      get_property(YARP_LIBRARIES GLOBAL PROPERTY YARP_LIBS)
      get_property(YARP_DEFINES GLOBAL PROPERTY YARP_DEFS)
    else (YARP_TREE_INCLUDE_DIRS)
      find_package(YARP REQUIRED)
    endif (YARP_TREE_INCLUDE_DIRS)

  ENDIF (YARPY_DEVICES)

ENDMACRO(BEGIN_DEVICE_LIBRARY devname)


#########################################################################
# ADD_DEVICE_NORMALIZED macro is an internal command to convert a 
# device declaration to code, and to set up CMake flags for controlling
# compilation of that device.  This macro is called be PREPARE_DEVICE
# which is the documented, user-facing macro.  PREPARE_DEVICE parses
# a flexible set of arguments, then passes them to ADD_DEVICE_NORMALIZED
# in a clean canonical order.
#
MACRO(ADD_DEVICE_NORMALIZED devname type include wrapper)

  # Append the current source directory to the set of include paths.
  # Developers seem to expect #include "foo.h" to work if foo.h is
  # in their module directory.
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  # Figure out a decent filename for the code we are about to 
  # generate.  If all else fails, the code will get dumped in
  # the current binary directory.
  SET(fdir ${YARPY_DEV_GEN})
  IF(NOT fdir)
    SET(fdir ${CMAKE_CURRENT_BINARY_DIR})
  ENDIF(NOT fdir)

  # We'll be expanding the code in template/yarpdev_helper.cpp.in using 
  # the following variables:

  SET(YARPDEV_NAME "${devname}")
  SET(YARPDEV_TYPE "${type}")
  SET(YARPDEV_INCLUDE "${include}")
  SET(YARPDEV_WRAPPER "${wrapper}")
  SET(ENABLE_YARPDEV_NAME "1")

  # Go ahead and prepare some code to wrap this device.  
  SET(fname ${fdir}/yarpdev_add_${devname}.cpp)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_helper.cpp.in
    ${fname} @ONLY  IMMEDIATE)
 
  # Set up a flag to enable/disable compilation of this device.
  SET(MYNAME "${DEVICE_PREFIX}${devname}")
  IF (NOT COMPILE_BY_DEFAULT)
    SET (COMPILE_BY_DEFAULT FALSE)
  ENDIF (NOT COMPILE_BY_DEFAULT)
  SET(ENABLE_${MYNAME} ${COMPILE_BY_DEFAULT} CACHE BOOL "Enable/disable compilation of ${MYNAME}")

  # Set some convenience variables based on whether the device
  # is enabled or disabled.
  SET(ENABLE_${devname} ${ENABLE_${MYNAME}})
  IF (ENABLE_${devname})
    SET(SKIP_${devname} FALSE)
    SET(SKIP_${MYNAME} FALSE)
  ELSE (ENABLE_${devname})
    SET(SKIP_${devname} TRUE)
    SET(SKIP_${MYNAME} TRUE)
  ENDIF (ENABLE_${devname})

  # If the device is enabled, add the appropriate source code into
  # the device library source list.
  IF (ENABLE_${MYNAME})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_DEVICES ${devname})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_CODE ${fname})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_INCLUDE_DIRS 
      ${CMAKE_CURRENT_SOURCE_DIR})
    SET(YARPY_DEV_ACTIVE TRUE)
    MESSAGE(STATUS " +++ device ${devname}, ENABLE_${devname} is set")
  ELSE (ENABLE_${MYNAME})
    MESSAGE(STATUS " +++ device ${devname}, SKIP_${devname} is set")
  ENDIF (ENABLE_${MYNAME})

  # We are done!

ENDMACRO(ADD_DEVICE_NORMALIZED devname type include wrapper)



#########################################################################
# PREPARE_DEVICE macro lets a developer declare a device using a 
# statement like:
#    PREPARE_DEVICE(foo TYPE FooDriver INCLUDE FooDriver.h)
# or
#    PREPARE_DEVICE(moto TYPE Moto INCLUDE moto.h WRAPPER controlboard)
# This macro is just a simple parser and calls ADD_DEVICE_NORMALIZED to
# do the actual work.
#
MACRO(PREPARE_DEVICE devname)
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
    IF(arg STREQUAL "TYPE")
      SET(EXPECT_TYPE TRUE)
    ENDIF(arg STREQUAL "TYPE")
    IF(arg STREQUAL "INCLUDE")
      SET(EXPECT_INCLUDE TRUE)
    ENDIF(arg STREQUAL "INCLUDE")
    IF(arg STREQUAL "WRAPPER")
      SET(EXPECT_WRAPPER TRUE)
    ENDIF(arg STREQUAL "WRAPPER")
  ENDFOREACH(arg ${ARGN})
  IF(THE_TYPE AND THE_INCLUDE)
    ADD_DEVICE_NORMALIZED(${devname} ${THE_TYPE} ${THE_INCLUDE} "${THE_WRAPPER}")
  ELSE(THE_TYPE AND THE_INCLUDE)
    MESSAGE(STATUS "Not enough information to create ${devname}")
    MESSAGE(STATUS "  type:    ${THE_TYPE}")
    MESSAGE(STATUS "  include: ${THE_INCLUDE}")
    MESSAGE(STATUS "  wrapper: ${THE_WRAPPER}")
  ENDIF(THE_TYPE AND THE_INCLUDE)
ENDMACRO(PREPARE_DEVICE devname)



#########################################################################
## Deprecated
MACRO(TARGET_IMPORT_DEVICES target hdr)
  MESSAGE(STATUS "[TARGET_]IMPORT_DEVICES macro has been deprecated")
  MESSAGE(STATUS "Instead just link against the device library, and in code do:")
  MESSAGE(STATUS "#include <yarp/dev/all.h>")
  MESSAGE(STATUS "YARP_DECLARE_DEVICES(${target});")
  MESSAGE(STATUS "...")
  MESSAGE(STATUS "   Network yarp;")
  MESSAGE(STATUS "   YARP_REGISTER_DEVICES(${devices});")
ENDMACRO(TARGET_IMPORT_DEVICES target hdr)


#########################################################################
## Deprecated
MACRO(IMPORT_DEVICES hdr)
  TARGET_IMPORT_DEVICES(the_device_library_name ${hdr})
ENDMACRO(IMPORT_DEVICES hdr)



#########################################################################
# Lightly redefine ADD_LIBRARY
#
MACRO(ADD_LIBRARY LIBNAME)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _ADD_LIBRARY(${LIBNAME} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    set(IS_IMPORTED FALSE)
    foreach(arg ${ARGN})
      if ("${arg}" STREQUAL "IMPORTED")
        set(IS_IMPORTED TRUE)
      endif()
    endforeach()
    if (NOT IS_IMPORTED)
      _ADD_LIBRARY(${LIBNAME} ${ARGN})
      message(STATUS "Recording a device library, ${LIBNAME}")
      set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_LIBS ${LIBNAME})
    endif (NOT IS_IMPORTED)
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(ADD_LIBRARY LIBNAME)


#########################################################################
# YARP_PREPARE_DEVICES macro creates generated code for enumerating
# devices, and adds a library containing all device code.
#
MACRO(YARP_PREPARE_DEVICES)
  SET(YARP_LIB_NAME ${YARPY_MASTER_DEVICE})

  get_property(devs GLOBAL PROPERTY YARP_BUNDLE_DEVICES)
  SET(YARP_CODE_PRE)
  SET(YARP_CODE_POST)
  FOREACH(dev ${devs})
    SET(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern void add_${dev}();")
    SET(YARP_CODE_POST "${YARP_CODE_POST}\n    add_${dev}();")
  ENDFOREACH()

  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib.cpp.in
    ${YARPY_DEV_GEN}/add_${YARPY_MASTER_DEVICE}_devices.cpp @ONLY  IMMEDIATE)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib.h.in
    ${YARPY_DEV_GEN}/add_${YARPY_MASTER_DEVICE}_devices.h @ONLY  IMMEDIATE)

  get_property(code GLOBAL PROPERTY YARP_BUNDLE_CODE)
  get_property(dirs GLOBAL PROPERTY YARP_BUNDLE_INCLUDE_DIRS)
  include_directories(${YARP_INCLUDE_DIRS} ${dirs})
  _ADD_LIBRARY(${YARPY_MASTER_DEVICE} ${code} ${YARPY_DEV_GEN}/add_${YARPY_MASTER_DEVICE}_devices.cpp)
  target_link_libraries(${YARPY_MASTER_DEVICE} ${YARP_LIBRARIES})
  get_property(libs GLOBAL PROPERTY YARP_BUNDLE_LIBS)
  target_link_libraries(${YARPY_MASTER_DEVICE} ${libs})
  set(${YARPY_MASTER_DEVICE}_LIBRARIES ${libs})
ENDMACRO(YARP_PREPARE_DEVICES)


#########################################################################
# END_DEVICE_LIBRARY macro calls YARP_PREPARE_DEVICES if this is
# the outermost device library block, otherwise it propagates
# all collected information to the device library block that wraps
# it.
#
MACRO(END_DEVICE_LIBRARY devname)
  MESSAGE(STATUS "ending device library: ${devname}")
  IF ("${devname}" STREQUAL "${YARPY_MASTER_DEVICE}")
    YARP_PREPARE_DEVICES()   # generate device library
    SET(YARPY_DEVICES FALSE) # neutralize redefined methods 
  ENDIF ("${devname}" STREQUAL "${YARPY_MASTER_DEVICE}")
ENDMACRO(END_DEVICE_LIBRARY devname)

MACRO(ADD_DEVICE_LIBRARY_EXECUTABLE exename devname)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib_main.cpp.in
    ${YARPY_DEV_GEN}/yarpdev_${devname}.cpp @ONLY  IMMEDIATE)
    ADD_EXECUTABLE(${exename} ${YARPY_DEV_GEN}/yarpdev_${devname}.cpp)
    TARGET_LINK_LIBRARIES(${exename} ${devname})
ENDMACRO(ADD_DEVICE_LIBRARY_EXECUTABLE)


ENDIF (COMMAND END_DEVICE_LIBRARY)
