# Copyright: (C) 2010 RobotCub Consortium
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# check pkg-config, as per:
# File: [SVN] / trunk / KDE / kdesdk / cmake / modules / UsePkgConfig.cmake
# Revision: 501797, Mon Jan 23 22:03:18 2006 UTC by neundorf

FIND_PROGRAM(PKGCONFIG_EXECUTABLE NAMES pkg-config PATHS /usr/local/bin)
MARK_AS_ADVANCED(PKGCONFIG_EXECUTABLE)

MACRO(PKGCONFIG _package _include_DIR _link_DIR _link_FLAGS _cflags)
  SET(${_include_DIR})
  SET(${_link_DIR})
  SET(${_link_FLAGS})
  SET(${_cflags})

  IF(PKGCONFIG_EXECUTABLE)
    EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --exists RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
    IF(NOT _return_VALUE)
      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --variable=includedir OUTPUT_VARIABLE ${_include_DIR} )
      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --variable=libdir OUTPUT_VARIABLE ${_link_DIR} )
      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --libs-only-l --libs-only-other OUTPUT_VARIABLE ${_link_FLAGS} )
      EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS ${_package} --cflags OUTPUT_VARIABLE ${_cflags} )
    ENDIF(NOT _return_VALUE)
  ENDIF(PKGCONFIG_EXECUTABLE)
ENDMACRO(PKGCONFIG _include_DIR _link_DIR _link_FLAGS _cflags)

SET(ACE_INCLUDE_DIR_GUESS)
SET(ACE_LIBRARY_DIR_GUESS)
SET(ACE_LINK_FLAGS)
IF(PKGCONFIG_EXECUTABLE)
	PKGCONFIG(ace ACE_INCLUDE_DIR_GUESS ACE_LIBRARY_DIR_GUESS ACE_LINK_FLAGS ACE_C_FLAGS)
	IF (NOT ACE_LINK_FLAGS)
		PKGCONFIG(ACE ACE_INCLUDE_DIR_GUESS ACE_LIBRARY_DIR_GUESS ACE_LINK_FLAGS ACE_C_FLAGS)
	ENDIF (NOT ACE_LINK_FLAGS)
	ADD_DEFINITIONS(${ACE_C_FLAGS})
ENDIF(PKGCONFIG_EXECUTABLE)
SET(ACE_LINK_FLAGS "${ACE_LINK_FLAGS}" CACHE INTERNAL "ace link flags")


########################################################################
##  general find

FIND_PATH(ACE_INCLUDE_DIR ace/ACE.h ${CMAKE_SOURCE_DIR}/../ACE_wrappers/ ${CMAKE_SOURCE_DIR}/../../ACE_wrappers/ ${CMAKE_SOURCE_DIR}/../../../ACE_wrappers/ /usr/include /usr/local/include $ENV{ACE_ROOT} DOC "directory containing ace/*.h for ACE library")

SET(ACE_INCLUDE_CONFIG_DIR "" CACHE STRING "location of ace/config.h")
MARK_AS_ADVANCED(ACE_INCLUDE_CONFIG_DIR)

FIND_LIBRARY(ACE_LIBRARY NAMES ACE ace PATHS ${CMAKE_SOURCE_DIR}/../ACE_wrappers/lib/ ${CMAKE_SOURCE_DIR}/../../ACE_wrappers/lib/ ${CMAKE_SOURCE_DIR}/../../../ACE_wrappers/lib/ /usr/lib /usr/local/lib $ENV{ACE_ROOT}/lib $ENV{ACE_ROOT} DOC "ACE library file")

IF (WIN32 AND NOT CYGWIN)
	SET(CMAKE_DEBUG_POSTFIX "d")
	FIND_LIBRARY(ACE_DEBUG_LIBRARY NAMES ACE${CMAKE_DEBUG_POSTFIX} ace${CMAKE_DEBUG_POSTFIX} PATHS ${CMAKE_SOURCE_DIR}/../ACE_wrappers/lib/ ${CMAKE_SOURCE_DIR}/../../ACE_wrappers/lib/ ${CMAKE_SOURCE_DIR}/../../../ACE_wrappers/lib/ /usr/lib /usr/local/lib $ENV{ACE_ROOT}/lib $ENV{ACE_ROOT} DOC "ACE library file (debug version)")
ENDIF (WIN32 AND NOT CYGWIN)


########################################################################
## finished - now just set up flags and complain to user if necessary

IF (ACE_INCLUDE_DIR AND ACE_LIBRARY)
	SET(ACE_FOUND TRUE)
ELSE (ACE_INCLUDE_DIR AND ACE_LIBRARY)
	SET(ACE_FOUND FALSE)
ENDIF (ACE_INCLUDE_DIR AND ACE_LIBRARY)

IF (ACE_DEBUG_LIBRARY)
	SET(ACE_DEBUG_FOUND TRUE)
ENDIF (ACE_DEBUG_LIBRARY)

IF (ACE_DEBUG_FOUND)
 LINK_LIBRARIES(optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY})
 SET(ACE_LIB "optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY}" CACHE INTERNAL "libraries")
ELSE (ACE_DEBUG_FOUND)
 LINK_LIBRARIES(${ACE_LIBRARY})
 SET(ACE_LIB "${ACE_LIBRARY}" CACHE INTERNAL "libraries")
ENDIF (ACE_DEBUG_FOUND)

IF (WIN32 AND NOT CYGWIN)
  LINK_LIBRARIES(winmm)
  SET(ACE_LIB "${ACE_LIB} winmm" CACHE INTERNAL "libraries")
ENDIF (WIN32 AND NOT CYGWIN)

IF (UNIX)
  LINK_LIBRARIES(pthread)
ENDIF (UNIX)

IF (ACE_FOUND)
	INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIR})
	LINK_LIBRARIES(${ACE_LINK_FLAGS})

	# on windows, we have to tell ace how it was compiled
	IF (WIN32 AND NOT CYGWIN)
	   ADD_DEFINITIONS(-DWIN32 -D_WINDOWS)
	ELSE (WIN32 AND NOT CYGWIN)
	   # flush out warnings
	   ADD_DEFINITIONS(-Wall)
	ENDIF (WIN32 AND NOT CYGWIN)

	IF (NOT Ace_FIND_QUIETLY)
		MESSAGE(STATUS "Found ACE library: ${ACE_LIBRARY}")
		MESSAGE(STATUS "Found ACE include: ${ACE_INCLUDE_DIR}")
	ENDIF (NOT Ace_FIND_QUIETLY)
ELSE (ACE_FOUND)
	IF (Ace_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find ACE")
	ENDIF (Ace_FIND_REQUIRED)
ENDIF (ACE_FOUND)
