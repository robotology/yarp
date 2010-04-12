
#########################################################################
# BEGIN_DEVICE_LIBRARY macro makes sure that all the cmake hooks
# needed for creating a device library are in place.  Those hooks
# are defined in YarpModuleHooks.cmake
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

    # If hooks have not been added, add them
    IF (NOT YARPY_DEVICES_INSTALLED)
      MESSAGE(STATUS "adding hooks for device library compilation")
      SET(YARPY_DEVICES_INSTALLED TRUE)
      IF (NOT COMPILE_DEVICE_LIBRARY)
        INCLUDE(YarpModuleHooks2)
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

    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_LIBS)
    SET_PROPERTY(GLOBAL PROPERTY YARP_BUNDLE_CODE)

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

  MESSAGE(STATUS " + Generating code for device [${devname}] type [${type}] include [${include}] wrapper [${wrapper}]")

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
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_CODE ${fname})
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

