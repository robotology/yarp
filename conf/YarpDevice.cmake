
#########################################################################
##
## Yet Another Device Compiling Monster
##
## This file, along with YarpModuleHooks.cmake, provides a set of macros
## for building bundles of devices.
##  BEGIN_DEVICE_LIBRARY(libname)
##  END_DEVICE_LIBRARY(libname)
## etc.
##
## CMake commands that lie between BEGIN_DEVICE_LIBRARY and 
## END_DEVICE_LIBRARY will be intercepted (see YarpModuleHooks.cmake)
## and any source files that reference, libraries they link, directories
## they add to the include path, and definitions they add will all be
## put in a big list and used to compile a single library called 
## <libname>.
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
## which calls add the add_*() methods for devices within that bundle.
##

#########################################################################
# BEGIN_DEVICE_LIBRARY macro makes sure that all the cmake hooks
# needed for creating a device library are in place.  Those hooks
# are defined in YarpModuleHooks.cmake
#
MACRO(BEGIN_DEVICE_LIBRARY devname)

  # YARP device library bundles should take a "d" suffix for debug
  # versions.
  IF (MSVC)
    SET(CMAKE_DEBUG_POSTFIX "d")
  ENDIF (MSVC)

  # Dependencies on YARP of individual device modules via
  # FIND_PACKAGE(YARP) will be optimized by substituting directly
  # the names of the YARP libraries.
  INCLUDE(YarpLibraryNames)

  # This is a collection of variable used to track all sorts of
  # resources as a device module is created.  For example, when
  # in a device module there is a line like:
  #    LINK_LIBRARIES(foo bar)
  # then instead of those libraries being linked, they are
  # appended to one of these variables.  Similarly for definitions,
  # include paths, source files, etc.  In the end, a single 
  # library target is created based on all this material.
  # This has the down-side that device module CMakeLists.txt
  # files should not get too fancy, and the up-side that
  # those files can be read and written with basic knowledge
  # of CMake.
  SET(YARPY_LIB_LIST0) # List of libraries linked
  SET(YARPY_SRC_LIST0) # List of source files added
  SET(YARPY_INC_LIST0) # List of include directories added
  SET(YARPY_LNK_LIST0) # List of link directories added
  SET(YARPY_DEF_LIST0) # List of definitions added
  SET(YARPY_DEV_LIST0) # List of device names

  # If we are nested inside a larger device block, we don't
  # have to do much.  If we are the outermost device block,
  # then we need to set up everything.
  IF (YARPY_DEVICES)

    MESSAGE(STATUS "nested library ${devname}")
    SET(DEVICE_PREFIX "${devname}_")

  ELSE (YARPY_DEVICES)

    # Support pkgconfig usage
    INCLUDE(UsePkgConfig)

    # Record name of device, in case user wants to import it later
    # using TARGET_IMPORT_DEVICES
    SET(YARP_KNOWN_DEVICE_LIBS ${YARP_KNOWN_DEVICE_LIBS} ${devname})
    IF (COMPILING_ALL_YARP)
      SET(YARP_MODULE_PATH "${CMAKE_SOURCE_DIR}/conf")
    ELSE (COMPILING_ALL_YARP)
      IF (NOT YARP_MODULE_PATH)
        SET(YARP_MODULE_PATH "${YARP_ROOT}/conf")
      ENDIF (NOT YARP_MODULE_PATH)
    ENDIF (COMPILING_ALL_YARP)

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

    SET(YARPY_DEV_LIST)
    SET(YARPY_DEV_SRC_LIST)

    # Check if hooks have been added
    IF (COMMAND END_DEVICE_LIBRARY)
      SET(YARPY_DEVICES_INSTALLED TRUE)
    ENDIF (COMMAND END_DEVICE_LIBRARY)

    # If hooks have not been added, add them
    IF (NOT YARPY_DEVICES_INSTALLED)
      MESSAGE(STATUS "adding hooks for device library compilation")
      SET(YARPY_DEVICES_INSTALLED TRUE)
      IF (NOT COMPILE_DEVICE_LIBRARY)
        INCLUDE(YarpModuleHooks)
      ENDIF (NOT COMPILE_DEVICE_LIBRARY)
    ENDIF (NOT YARPY_DEVICES_INSTALLED)

    # Set a flag to let individual modules know that they are being
    # compiled as part of a bundle, and not standalone.  Developers
    # use this flag to inhibit compilation of test programs and 
    # the like.
    SET(COMPILE_DEVICE_LIBRARY TRUE)

    # Record the name of this outermost device bundle (needed because
    # nesting is allowed)
    SET(YARPY_MASTER_DEVICE ${devname})

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
    SET(YARPY_DEV_SRC_LIST ${YARPY_DEV_SRC_LIST} ${fname})
    SET(YARPY_DEV_LIST ${YARPY_DEV_LIST} ${devname})
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
# IMPORT_DEVICES macro will link named device libraries, and generate
# a header file that makes all its devices accessible.
#
MACRO(IMPORT_DEVICES hdr)
  FOREACH (libname ${ARGN})
    IF (NOT COMPILING_ALL_YARP)
      SET(KNOWN FALSE)
      FOREACH (klibname ${YARP_KNOWN_DEVICE_LIBS})
        IF ("${klibname}" STREQUAL "${libname}")
          SET(KNOWN TRUE)
        ENDIF ("${klibname}" STREQUAL "${libname}")
      ENDFOREACH (klibname ${YARP_KNOWN_DEVICE_LIBS})
      IF (KNOWN)
        LINK_LIBRARIES(${libname})
      ELSE (KNOWN)
        FIND_PACKAGE(${libname})
      ENDIF (KNOWN)
    ENDIF (NOT COMPILING_ALL_YARP)

    IF(MSVC)
      LINK_LIBRARIES(optimized ${libname} debug ${libname}d)
    ELSE(MSVC)
      LINK_LIBRARIES(optimized ${libname} debug ${libname})
    ENDIF(MSVC)
    
  ENDFOREACH (libname ${ARGN})

  SET(YARP_CODE_PRE)
  SET(YARP_CODE_POST)
  FOREACH(dev ${ARGN})
    SET(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern void add_${dev}_devices();")
    SET(YARP_CODE_POST "${YARP_CODE_POST}\n        add_${dev}_devices();")
  ENDFOREACH(dev ${})
  SET(YARP_LIB_NAME ${YARPY_DEV_LIB_NAME})
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_import.h.in
    ${hdr} @ONLY  IMMEDIATE)
  MESSAGE(STATUS "generated ${hdr}")
  IF (YARP_LIBRARIES)
    LINK_LIBRARIES(${YARP_LIBRARIES})
  ELSE (YARP_LIBRARIES)
    LINK_LIBRARIES(YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)
  ENDIF (YARP_LIBRARIES)
ENDMACRO(IMPORT_DEVICES hdr)



#########################################################################
# TARGET_IMPORT_DEVICES macro will link named device libraries against
# a specific target, and generated a header file that makes all its 
# devices accessible.
#
MACRO(TARGET_IMPORT_DEVICES target hdr)
  FOREACH (libname ${ARGN})
    IF (NOT COMPILING_ALL_YARP)
      SET(KNOWN FALSE)
      FOREACH (klibname ${YARP_KNOWN_DEVICE_LIBS})
        IF ("${klibname}" STREQUAL "${libname}")
          SET(KNOWN TRUE)
        ENDIF ("${klibname}" STREQUAL "${libname}")
      ENDFOREACH (klibname ${YARP_KNOWN_DEVICE_LIBS})
      IF (NOT KNOWN)
        FIND_PACKAGE(${libname})
      ENDIF (NOT KNOWN)
    ENDIF (NOT COMPILING_ALL_YARP)
    TARGET_LINK_LIBRARIES(${target} ${libname})
  ENDFOREACH (libname ${ARGN})
  SET(YARP_CODE_PRE)
  SET(YARP_CODE_POST)
  FOREACH(dev ${ARGN})
    SET(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern void add_${dev}_devices();")
    SET(YARP_CODE_POST "${YARP_CODE_POST}\n        add_${dev}_devices();")
  ENDFOREACH(dev ${})
  SET(YARP_LIB_NAME ${YARPY_DEV_LIB_NAME})
  CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev_import.h.in
    ${hdr} @ONLY  IMMEDIATE)
  MESSAGE(STATUS "generated ${hdr}")
  IF (YARP_LIBRARIES)
    TARGET_LINK_LIBRARIES(${target} ${YARP_LIBRARIES})
  ELSE (YARP_LIBRARIES)
    TARGET_LINK_LIBRARIES(${target} YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)
  ENDIF (YARP_LIBRARIES)
ENDMACRO(TARGET_IMPORT_DEVICES target hdr)
