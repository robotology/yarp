

########################################################################
## check pkg-config for ace information, if available

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

FIND_PATH(ACE_INCLUDE_DIR ace/ACE.h ${CMAKE_SOURCE_DIR}/../ACE_wrappers/ /usr/include /usr/local/include $ENV{ACE_ROOT} $ENV{ACE_ROOT}/include DOC "directory containing ace/*.h for ACE library")

# in YARP1, config was in another directory
SET(ACE_INCLUDE_CONFIG_DIR "" CACHE STRING "location of ace/config.h")
MARK_AS_ADVANCED(ACE_INCLUDE_CONFIG_DIR)

FIND_LIBRARY(ACE_LIBRARY NAMES ACE ace PATHS ${CMAKE_SOURCE_DIR}/../ACE_wrappers/lib/ /usr/lib /usr/local/lib $ENV{ACE_ROOT}/lib $ENV{ACE_ROOT} DOC "ACE library file")

IF (WIN32 AND NOT CYGWIN)
	SET(CMAKE_DEBUG_POSTFIX "d")
	FIND_LIBRARY(ACE_DEBUG_LIBRARY NAMES ACE${CMAKE_DEBUG_POSTFIX} ace${CMAKE_DEBUG_POSTFIX} PATHS ${CMAKE_SOURCE_DIR}/../ACE_wrappers/lib/ /usr/lib /usr/local/lib $ENV{ACE_ROOT}/lib $ENV{ACE_ROOT} DOC "ACE library file (debug version)")
ENDIF (WIN32 AND NOT CYGWIN)


########################################################################
## OS-specific extra linkage

# Solaris needs some extra libraries that may not have been found already
IF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
  MESSAGE(STATUS "need to link solaris-specific libraries")
  #  LINK_LIBRARIES(socket rt)
  SET(ACE_LIBRARY ${ACE_LIBRARY} socket rt nsl)
ENDIF(CMAKE_SYSTEM_NAME STREQUAL "SunOS")

# Windows needs some extra libraries
IF (WIN32 AND NOT CYGWIN)
  MESSAGE(STATUS "need to link windows-specific libraries")
  #LINK_LIBRARIES(winmm)
  SET(ACE_LIBRARY ${ACE_LIBRARY} winmm)
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

IF (ACE_FOUND)
	IF (NOT Ace_FIND_QUIETLY)
		MESSAGE(STATUS "Found ACE library: ${ACE_LIBRARY}")
		MESSAGE(STATUS "Found ACE include: ${ACE_INCLUDE_DIR}")
	ENDIF (NOT Ace_FIND_QUIETLY)
ELSE (ACE_FOUND)
	IF (Ace_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find ACE")
	ENDIF (Ace_FIND_REQUIRED)
ENDIF (ACE_FOUND)
