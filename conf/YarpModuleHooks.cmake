# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

#########################################################################
##
## Yet Another Device Compiling Monster
##
## The following CMake commands are redefined during creation of a 
## device library:
##
##   ADD_LIBRARY
##   SET_TARGET_PROPERTIES
##   ADD_EXECUTABLE
##   ADD_DEFINITIONS
##   LINK_LIBRARIES
##   FIND_PACKAGE
##   ADD_SUBDIRECTORY
##
## The action they take is unchanged outside of device compilation.
## Within device compilation, the action taken by these commands is
## as close to the original as possible, with the constraint that
## the result from all code between:
##   BEGIN_DEVICE_LIBRARY(libname)
## and 
##   END_DEVICE_LIBRARY(libname) 
## will be a single library called "libname".  Executables are 
## ignored, and the source code for multiple libraries is merged.
##
## BEGIN_DEVICE_LIBRARY/END_DEVICE_LIBRARY calls can be nested,
## with the result still being a single library.
##
## The "COMPILE_DEVICE_LIBRARY" flag is true between:
##   BEGIN_DEVICE_LIBRARY 
## and 
##   END_DEVICE_LIBRARY
## commands.
##
##
##
#########################################################################


#########################################################################
# Redefine ADD_LIBRARY
#
MACRO(ADD_LIBRARY LIBNAME)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _ADD_LIBRARY(${LIBNAME} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    IF (YARPY_DEV_ACTIVE)
      SET(YARPY_ACTIVE TRUE)
      FOREACH(SRC ${ARGN})
        GET_SOURCE_FILE_PROPERTY(SRC2 ${SRC} LOCATION)
        SET(YARPY_SRC_LIST ${YARPY_SRC_LIST} ${SRC2})
      ENDFOREACH(SRC ${ARGN})
      SET(YARPY_SRC_LIST ${YARPY_SRC_LIST} ${YARPY_DEV_SRC_LIST})
    ELSE (YARPY_DEV_ACTIVE)
      SET(YARPY_SRC_LIST)
      SET(YARPY_DEV_LIST)
    ENDIF (YARPY_DEV_ACTIVE)
    SET(YARPY_DEV_SRC_LIST)
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(ADD_LIBRARY LIBNAME)


#########################################################################
# Redefine SET_TARGET_PROPERTIES
#
MACRO(SET_TARGET_PROPERTIES LIBNAME)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _SET_TARGET_PROPERTIES(${LIBNAME} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    # the names of targets are modified
    SET(MYNAME "${DEVICE_PREFIX}${LIBNAME}")
    _SET_TARGET_PROPERTIES(${MYNAME} ${ARGN})
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(SET_TARGET_PROPERTIES LIBNAME)


#########################################################################
# Redefine ADD_EXECUTABLE
#
MACRO(ADD_EXECUTABLE EXENAME)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _ADD_EXECUTABLE(${EXENAME} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    # we do nothing with executables
    MESSAGE(STATUS "ignoring executable ${EXENAME}: ${ARGN}")
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(ADD_EXECUTABLE EXENAME)


#########################################################################
# Redefine ADD_DEFINITIONS
#
MACRO(ADD_DEFINITIONS)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _ADD_DEFINITIONS(${EXENAME} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    MESSAGE(STATUS "adding definitions: ${ARGN}")
    _ADD_DEFINITIONS(${ARGN})
    SET(YARPY_DEF_LIST ${YARPY_DEF_LIST} ${ARGN})
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(ADD_DEFINITIONS)


#########################################################################
# Redefine LINK_LIBRARIES
#
MACRO(LINK_LIBRARIES)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _LINK_LIBRARIES(${ARGN})
  ELSE (NOT YARPY_DEVICES)
    _LINK_LIBRARIES(${ARGN})
    SET(YARPY_LIB_LIST ${YARPY_LIB_LIST} ${ARGN})
	MESSAGE("YARPY_LIB_LIST ${YARPY_LIB_LIST}")
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(LINK_LIBRARIES)


#########################################################################
# Redefine TARGET_LINK_LIBRARIES
#
MACRO(TARGET_LINK_LIBRARIES target)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _TARGET_LINK_LIBRARIES(${target} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    _LINK_LIBRARIES(${ARGN})
    SET(YARPY_LIB_LIST ${YARPY_LIB_LIST} ${ARGN})
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(TARGET_LINK_LIBRARIES)


#########################################################################
# Redefine FIND_PACKAGE
#
MACRO(FIND_PACKAGE pkg)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _FIND_PACKAGE(${pkg} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    # if the package we are looking for is YARP, insert dependencies
    # directly
    IF (NOT "${pkg}" STREQUAL "YARP")
      _FIND_PACKAGE(${pkg} ${ARGN})
    ELSE (NOT "${pkg}" STREQUAL "YARP")
      IF (COMPILING_ALL_YARP)
    	INCLUDE_DIRECTORIES(${YARP_OS_INC} ${YARP_sig_INC} ${YARP_dev_INC})
	LINK_LIBRARIES(${YARP_LIBRARIES})
	SET(YARP_FOUND TRUE)
      ELSE (COMPILING_ALL_YARP)
 	_FIND_PACKAGE(${pkg} ${ARGN})
      ENDIF (COMPILING_ALL_YARP)
    ENDIF (NOT "${pkg}" STREQUAL "YARP")
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(FIND_PACKAGE)


#########################################################################
# Redefine ADD_SUBDIRECTORY
#
MACRO(ADD_SUBDIRECTORY SUBDIR)
  IF (NOT YARPY_DEVICES)
    # pass on call without looking at it
    _ADD_SUBDIRECTORY(${SUBDIR} ${ARGN})
  ELSE (NOT YARPY_DEVICES)
    _ADD_SUBDIRECTORY(${SUBDIR} ${ARGN})
    # After the subdirectory has been added, we need to pull out
    # the list variables prepared in it for the libraries to link,
    # include paths to add, etc.  CMake isolates those variables
    # to the directory they occur in, which is nice in general
    # but not what we want in this case.
    GET_DIRECTORY_PROPERTY(LIBS DIRECTORY ${SUBDIR} DEFINITION YARPY_LIB_LIST)
    GET_DIRECTORY_PROPERTY(SRCS DIRECTORY ${SUBDIR} DEFINITION YARPY_SRC_LIST)
    GET_DIRECTORY_PROPERTY(INCS DIRECTORY ${SUBDIR} INCLUDE_DIRECTORIES)
    GET_DIRECTORY_PROPERTY(INCS2 DIRECTORY ${SUBDIR} DEFINITION YARPY_INC_LIST)
    IF (INCS2)
      SET(INCS ${INCS2})
    ENDIF (INCS2)
    GET_DIRECTORY_PROPERTY(LNKS DIRECTORY ${SUBDIR} LINK_DIRECTORIES)
    GET_DIRECTORY_PROPERTY(LNKS2 DIRECTORY ${SUBDIR} DEFINITION YARPY_LNK_LIST)
    IF (LNKS2)
      SET(LNKS ${LNKS2})
    ENDIF (LNKS2)
    GET_DIRECTORY_PROPERTY(DEFS DIRECTORY ${SUBDIR} DEFINITION YARPY_DEF_LIST)
    GET_DIRECTORY_PROPERTY(DEVS DIRECTORY ${SUBDIR} DEFINITION YARPY_DEV_LIST)
    GET_DIRECTORY_PROPERTY(ACTS DIRECTORY ${SUBDIR} DEFINITION YARPY_ACTIVE)
    IF (ACTS)
      SET(YARPY_LIB_LIST0 ${YARPY_LIB_LIST0} ${LIBS})
      SET(YARPY_SRC_LIST0 ${YARPY_SRC_LIST0} ${SRCS})
      SET(YARPY_INC_LIST0 ${YARPY_INC_LIST0} ${INCS})
      SET(YARPY_LNK_LIST0 ${YARPY_LNK_LIST0} ${LNKS})
      SET(YARPY_DEF_LIST0 ${YARPY_DEF_LIST0} ${DEFS})
    ENDIF (ACTS)
    SET(YARPY_DEV_LIST0 ${YARPY_DEV_LIST0} ${DEVS})
  ENDIF (NOT YARPY_DEVICES)
ENDMACRO(ADD_SUBDIRECTORY SUBDIR)


#########################################################################
# YARP_PREPARE_DEVICES macro creates generated code for enumerating
# devices, and adds a library containing all device code.
#
MACRO(YARP_PREPARE_DEVICES)
  SET(YARP_CODE_PRE)
  SET(YARP_CODE_POST)
  FOREACH(dev ${YARPY_DEV_LIST0})
    SET(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern void add_${dev}();")
    SET(YARP_CODE_POST "${YARP_CODE_POST}\n    add_${dev}();")
  ENDFOREACH(dev ${YARPY_DEV_LIST0})
  SET(YARP_LIB_NAME ${YARPY_DEV_LIB_NAME})
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib.cpp.in
    ${YARPY_DEV_GEN}/add_${YARP_LIB_NAME}_devices.cpp @ONLY  IMMEDIATE)
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib.h.in
    ${CMAKE_BINARY_DIR}/add_${YARP_LIB_NAME}_devices.h @ONLY  IMMEDIATE)

  SET(YARPY_DEV_LIBRARIES)
  INCLUDE_DIRECTORIES(${YARPY_INC_LIST0})
  LINK_DIRECTORIES(${YARPY_LNK_LIST0})
  _ADD_DEFINITIONS(${YARPY_DEF_LIST0})
  _LINK_LIBRARIES(${YARPY_LIB_LIST0})
  _ADD_LIBRARY(${YARPY_DEV_LIB_NAME} ${YARPY_SRC_LIST0} ${YARPY_DEV_GEN}/add_${YARPY_DEV_LIB_NAME}_devices.cpp ${MERGE_SRCS})
  SET(YARPY_DEV_LIBRARIES ${YARPY_DEV_LIB_NAME} ${YARPY_LIB_LIST0})
  GET_TARGET_PROPERTY(YARP_DEV_LIB_LOCATION ${YARPY_DEV_LIB_NAME} LOCATION)
  GET_FILENAME_COMPONENT(YARP_DEV_LIB_DIR ${YARP_DEV_LIB_LOCATION} PATH)

  IF (COMPILING_ALL_YARP)
  ELSE (COMPILING_ALL_YARP)
    CONFIGURE_FILE(${YARP_MODULE_PATH}/template/yarpdev_lib_main.cpp.in
      ${YARPY_DEV_GEN}/yarpdev_${YARPY_DEV_LIB_NAME}.cpp @ONLY  IMMEDIATE)
    _ADD_EXECUTABLE(${YARPY_DEV_LIB_NAME}dev ${YARPY_DEV_GEN}/yarpdev_${YARPY_DEV_LIB_NAME}.cpp)

    SET(YARPY_DEV_LIB_DEP optimized ${YARPY_DEV_LIB_NAME} debug ${YARPY_DEV_LIB_NAME})
    _TARGET_LINK_LIBRARIES(${YARPY_DEV_LIB_NAME}dev ${YARPY_DEV_LIB_DEP})

  ENDIF (COMPILING_ALL_YARP)

  SET(YARPY_DEV_DIR ${CMAKE_BINARY_DIR})
  SET(YARPY_OUT_DIR ${CMAKE_BINARY_DIR})
  CONFIGURE_FILE(${YARP_MODULE_PATH}/template/DEVICEConfig.cmake.in
    ${CMAKE_BINARY_DIR}/${YARPY_DEV_LIB_NAME}Config.cmake @ONLY  IMMEDIATE)
  EXPORT_LIBRARY_DEPENDENCIES(${CMAKE_BINARY_DIR}/${YARPY_DEV_LIB_NAME}Dependencies.cmake)

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
  ELSE ("${devname}" STREQUAL "${YARPY_MASTER_DEVICE}")

    # this is a nested device library, propagate information out.
    SET(YARPY_LIB_LIST ${YARPY_LIB_LIST0})
    SET(YARPY_SRC_LIST ${YARPY_SRC_LIST0})
    SET(YARPY_INC_LIST ${YARPY_INC_LIST0})
    SET(YARPY_LNK_LIST ${YARPY_LNK_LIST0})
    SET(YARPY_DEF_LIST ${YARPY_DEF_LIST0})
    SET(YARPY_DEV_LIST ${YARPY_DEV_LIST0})
    SET(YARPY_ACTIVE TRUE)

  ENDIF ("${devname}" STREQUAL "${YARPY_MASTER_DEVICE}")
ENDMACRO(END_DEVICE_LIBRARY devname)

