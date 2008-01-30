MACRO(YarpDevice)

INCLUDE(UsePkgConfig)

SET(GEN ${CMAKE_BINARY_DIR}/generated_code)
IF (NOT EXISTS ${GEN})
	FILE(MAKE_DIRECTORY ${GEN})
ENDIF (NOT EXISTS ${GEN})

SET(YARP_KNOWN_DEVICE_LIBS)

# We have a cpp file and a header file that call/list the 
# initialization methods for all devices
SET(ADDER_CPP ${GEN}/adder.cpp)
SET(ADDER_H ${GEN}/adder.h)

# Write some preamble for the cpp file and header file
WRITE_FILE(${ADDER_CPP} "#include \"adder.h\"")
WRITE_FILE(${ADDER_CPP} "void adder() {" APPEND)
WRITE_FILE(${ADDER_H} "")

# For each device directory, create the appropriate files
FOREACH(dev ${ARGN})
	MESSAGE(STATUS "Dealing with device ${dev}")

	# pick up the configuration of the device
	GET_FILENAME_COMPONENT(dev_path "${dev}" PATH)
	SET(SAVE_PATH ${CMAKE_MODULE_PATH})
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${dev_path})
	IF (dev_path)
		INCLUDE(${dev_path}/yarpdevice.cmake)
	ELSE (dev_path)
		INCLUDE(${dev})
	ENDIF (dev_path)
	SET(CMAKE_MODULE_PATH ${SAVE_PATH})

	# make a flag for conditional compilation of the device
	SET(ENABLE_${YARPDEV_NAME} FALSE CACHE BOOL "Do you want to use ${YARPDEV_NAME}?")
	SET(ENABLE_YARPDEV_NAME 0)
	IF (ENABLE_${YARPDEV_NAME})
	  SET(ENABLE_YARPDEV_NAME 1)
	ENDIF (ENABLE_${YARPDEV_NAME})

	IF (ENABLE_YARPDEV_NAME)
		# write a quick cpp file to add an appropriate factory for the device
		CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev_helper.cpp.in
		  ${GEN}/add_${YARPDEV_NAME}.cpp @ONLY  IMMEDIATE)
		MESSAGE(STATUS "Generated add_${YARPDEV_NAME}.cpp")

		# aggregate this into our global list
		WRITE_FILE(${ADDER_CPP} "add_${YARPDEV_NAME}();" APPEND)
		WRITE_FILE(${ADDER_H} "extern void add_${YARPDEV_NAME}();" APPEND)
		FILE(GLOB_RECURSE folder_dev_source ${dev_path}/*.cpp ${dev_path}/*.cc 
			${dev_path}/*.c)
	    	FILE(GLOB_RECURSE folder_dev_header ${dev_path}/*.h)
	
		SET(folder_source ${folder_source} ${folder_dev_source}
			${GEN}/add_${YARPDEV_NAME}.cpp)
		SET(folder_header ${folder_header} ${folder_dev_header})
	
		# make sure this device directory is included in our header
		# file path
		INCLUDE_DIRECTORIES(${dev_path})
	ELSE (ENABLE_YARPDEV_NAME)
		MESSAGE(STATUS "Device ${YARPDEV_NAME} is not enabled")	
	ENDIF (ENABLE_YARPDEV_NAME)
ENDFOREACH(dev)


# finish up the list of devices
WRITE_FILE(${ADDER_CPP} "}" APPEND)
MESSAGE(STATUS "Generated ${ADDER_CPP}")
WRITE_FILE(${ADDER_H} "extern void adder();" APPEND)
MESSAGE(STATUS "Generated ${ADDER_H}")
SET(folder_source ${folder_source} ${ADDER_CPP})
SET(folder_header ${folder_header} ${ADDER_H})

# write a standard wrapper program for creating the devices
CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev.cpp.in
	${GEN}/yarpdev.cpp @ONLY  IMMEDIATE)
MESSAGE(STATUS "Generated yarpdev.cpp")
SET(folder_source ${folder_source} ${GEN}/yarpdev.cpp)

# we are done!

ENDMACRO(YarpDevice)

MACRO(BEGIN_DEVICE_LIBRARY devname)

  INCLUDE(YarpLibraryNames)

  # reset dependency tracker
  SET(YARPY_LIB_LIST0)
  SET(YARPY_XLIB_LIST0)
  SET(YARPY_SRC_LIST0)
  SET(YARPY_INC_LIST0)
  SET(YARPY_LNK_LIST0)
  SET(YARPY_DEF_LIST0)
  SET(YARPY_DEV_LIST0)

  IF (NOT YARPY_DEVICES)
    INCLUDE(UsePkgConfig)
    SET(YARP_KNOWN_DEVICE_LIBS ${YARP_KNOWN_DEVICE_LIBS} ${devname})
    IF (COMPILING_ALL_YARP)
      SET(YARP_MODULE_PATH "${CMAKE_SOURCE_DIR}/conf")
    ELSE (COMPILING_ALL_YARP)
      SET(YARP_MODULE_PATH "${YARP_DIR}/conf")
    ENDIF (COMPILING_ALL_YARP)
    MESSAGE(STATUS "starting device library: ${devname}")
    SET(YARPY_DEV_LIB_NAME ${devname})
    SET(YARPY_DEVICES TRUE)

    FOREACH(arg ${ARGN})
      SET(MERGE_DEVICE_LIBS_INIT FALSE)
      IF (arg STREQUAL "MERGE")
        SET(MERGE_DEVICE_LIBS_INIT TRUE)
      ENDIF (arg STREQUAL "MERGE")
    ENDFOREACH(arg ${ARGN})
    # commands seem to lurk even if variables are removed
    IF (COMMAND END_DEVICE_LIBRARY)
      SET(YARPY_DEVICES_INSTALLED TRUE)
    ENDIF (COMMAND END_DEVICE_LIBRARY)

    SET(YARPY_DEV_GEN ${CMAKE_BINARY_DIR}/generated_code)
    IF (NOT EXISTS ${YARPY_DEV_GEN})
  	FILE(MAKE_DIRECTORY ${YARPY_DEV_GEN})
    ENDIF (NOT EXISTS ${YARPY_DEV_GEN})
    # Choose whether to merge library source code.
    # ---> now always merge.
    SET(MERGE_DEVICE_LIBS TRUE)
    #SET(MERGE_DEVICE_LIBS ${MERGE_DEVICE_LIBS_INIT} CACHE BOOL "Try to pack device libraries together a bit")
    SET(DEVICE_PREFIX "${devname}_")
    #SET(DEVICE_PREFIX "yarpdev_")
    SET(YARPY_LIB_FLAG EXCLUDE_FROM_ALL)
    IF (NOT YARPY_DEV_LIB_NAME)
      SET(YARPY_DEV_LIB_NAME devices)
    ENDIF (NOT YARPY_DEV_LIB_NAME)

    SET(YARPY_DEV_LIST)
    SET(YARPY_DEV_SRC_LIST)

    IF (NOT YARPY_DEVICES_INSTALLED)
      MESSAGE(STATUS "adding hooks for device library compilation")
      SET(YARPY_DEVICES_INSTALLED TRUE)
      INCLUDE(YarpModuleHooks)
    ENDIF (NOT YARPY_DEVICES_INSTALLED)

    SET(COMPILE_DEVICE_LIBRARY TRUE)

    SET(YARPY_MASTER_DEVICE ${devname})

  ELSE (NOT YARPY_DEVICES)

    MESSAGE(STATUS "nested library ${devname}")
    SET(DEVICE_PREFIX "${devname}_")

  ENDIF (NOT YARPY_DEVICES)

ENDMACRO(BEGIN_DEVICE_LIBRARY devname)

MACRO(ADD_DEVICE_NORMALIZED devname type include wrapper)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  SET(YARPDEV_NAME "${devname}")
  SET(YARPDEV_TYPE "${type}")
  SET(YARPDEV_INCLUDE "${include}")
  SET(YARPDEV_WRAPPER "${wrapper}")
  SET(ENABLE_YARPDEV_NAME "1")
  SET(fdir ${YARPY_DEV_GEN})
  IF(NOT fdir)
    SET(fdir ${CMAKE_CURRENT_BINARY_DIR})
  ENDIF(NOT fdir)
  SET(fname ${fdir}/yarpdev_add_${devname}.cpp)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev_helper.cpp.in
     ${fname} @ONLY  IMMEDIATE)
  ###MESSAGE(STATUS "Device ${devname} creation code in ${fname}")

  SET(MYNAME "${DEVICE_PREFIX}${devname}")
  IF (NOT COMPILE_BY_DEFAULT)
    SET (COMPILE_BY_DEFAULT FALSE)
  ENDIF (NOT COMPILE_BY_DEFAULT)
  SET(ENABLE_${MYNAME} ${COMPILE_BY_DEFAULT} CACHE BOOL "Enable/disable compilation of ${MYNAME}")

  # for user's convience
  SET(ENABLE_${devname} ${ENABLE_${MYNAME}})
  IF (ENABLE_${devname})
    SET(SKIP_${devname} FALSE)
    SET(SKIP_${MYNAME} FALSE)
  ELSE (ENABLE_${devname})
    SET(SKIP_${devname} TRUE)
    SET(SKIP_${MYNAME} TRUE)
  ENDIF (ENABLE_${devname})

  IF (ENABLE_${MYNAME})
    SET(YARPY_DEV_SRC_LIST ${YARPY_DEV_SRC_LIST} ${fname})
    SET(YARPY_DEV_LIST ${YARPY_DEV_LIST} ${devname})
    SET(YARPY_DEV_ACTIVE TRUE)
    MESSAGE(STATUS " +++ device ${devname}, ENABLE_${devname} is set")
  ELSE (ENABLE_${MYNAME})
    MESSAGE(STATUS " +++ device ${devname}, SKIP_${devname} is set")
  ENDIF (ENABLE_${MYNAME})



ENDMACRO(ADD_DEVICE_NORMALIZED devname type include wrapper)

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
    LINK_LIBRARIES(${libname})
  ENDFOREACH (libname ${ARGN})
  SET(YARP_CODE_PRE)
  SET(YARP_CODE_POST)
#  FOREACH(dev ${YARP_DEVICE_LIST})
  FOREACH(dev ${ARGN})
    SET(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern void add_${dev}_devices();")
    SET(YARP_CODE_POST "${YARP_CODE_POST}\n        add_${dev}_devices();")
  ENDFOREACH(dev ${})
  SET(YARP_LIB_NAME ${YARPY_DEV_LIB_NAME})
  CONFIGURE_FILE(${YARP_MODULE_PATH}/yarpdev_import.h.in
     ${hdr} @ONLY  IMMEDIATE)
  MESSAGE(STATUS "generated ${hdr}")
  IF (YARP_LIBRARIES)
        LINK_LIBRARIES(${YARP_LIBRARIES})
  ELSE (YARP_LIBRARIES)
        LINK_LIBRARIES(YARP_dev ${YARP_EXTMOD_TARGETS} ${YARP_dev_EXT_LIBS} YARP_sig YARP_OS)
  ENDIF (YARP_LIBRARIES)
ENDMACRO(IMPORT_DEVICES hdr)


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
