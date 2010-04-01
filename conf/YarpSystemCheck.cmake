# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale, Alessandro Scalzo
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

#########################################################################
# Check whether system is big- or little- endian

INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
IF(${IS_BIG_ENDIAN})
  SET(YARP_BIG_ENDIAN 1)
ELSE(${IS_BIG_ENDIAN})
  SET(YARP_LITTLE_ENDIAN 1)
ENDIF(${IS_BIG_ENDIAN})

#########################################################################
# Find 16, 32, and 64 bit types, portably

INCLUDE(CheckTypeSize)

SET(YARP_INT16)
SET(YARP_INT32)
SET(YARP_FLOAT64)

CHECK_TYPE_SIZE("int" SIZEOF_INT)
CHECK_TYPE_SIZE("short" SIZEOF_SHORT)
IF(SIZEOF_INT EQUAL 4)
  SET(YARP_INT32 "int")
ELSE(SIZEOF_INT EQUAL 4)
  IF(SIZEOF_SHORT EQUAL 4)
    SET(YARP_INT32 "short")
  ELSE(SIZEOF_SHORT EQUAL 4)
    CHECK_TYPE_SIZE("long" SIZEOF_LONG)
    IF(SIZEOF_LONG EQUAL 4)
      SET(YARP_INT32 "long")
    ENDIF(SIZEOF_LONG EQUAL 4)
  ENDIF(SIZEOF_SHORT EQUAL 4)
ENDIF(SIZEOF_INT EQUAL 4)

IF(SIZEOF_SHORT EQUAL 2)
  SET(YARP_INT16 "short")
ELSE(SIZEOF_SHORT EQUAL 2)
  # Hmm - there's no other native type to get 16 bits
  # We will continue since most people using YARP do not need one.
  MESSAGE(STATUS "Warning: cannot find a 16 bit type on your system")
  MESSAGE(STATUS "Continuing...")
ENDIF(SIZEOF_SHORT EQUAL 2)

CHECK_TYPE_SIZE("double" SIZEOF_DOUBLE)
IF(SIZEOF_DOUBLE EQUAL 8)
  SET(YARP_FLOAT64 "double")
ELSE(SIZEOF_DOUBLE EQUAL 8)
  IF(SIZEOF_FLOAT EQUAL 8)
    SET(YARP_FLOAT64 "float")
  ENDIF(SIZEOF_FLOAT EQUAL 8)
ENDIF(SIZEOF_DOUBLE EQUAL 8)


#########################################################################
# Get GUI libraries if needed - this should move to GUI directories

IF (CREATE_GUIS)
  FIND_PACKAGE(GtkPlus)
  IF (NOT GtkPlus_FOUND)
	MESSAGE(STATUS " gtk+ not found, won't compile dependent tools")
  ENDIF (NOT GtkPlus_FOUND)

  FIND_PACKAGE(Gthread)
  IF (NOT Gthread_FOUND)
	MESSAGE(STATUS " gthread not found, won't compile dependent tools")
  ENDIF(NOT Gthread_FOUND)
ENDIF (CREATE_GUIS)


#########################################################################
# Set up compile flags, and configure ACE

SET(YARP_ADMIN "$ENV{YARP_ADMIN}")

IF (YARP_ADMIN)
  # be very serious about warnings if in admin mode
  ADD_DEFINITIONS(-Werror -Wfatal-errors)
ENDIF (YARP_ADMIN)

ADD_DEFINITIONS(-DYARP_PRESENT)
ADD_DEFINITIONS(-D_REENTRANT)
set_property(GLOBAL APPEND PROPERTY YARP_DEFS -D_REENTRANT)

# on windows, we have to tell ace how it was compiled
IF (WIN32 AND NOT CYGWIN)
  ADD_DEFINITIONS(-DWIN32 -D_WINDOWS)
ELSE (WIN32 AND NOT CYGWIN)
  ADD_DEFINITIONS(-Wall)
ENDIF (WIN32 AND NOT CYGWIN)

## check if we are on cygwin
IF(WIN32 AND CYGWIN)
  ADD_DEFINITIONS(-DCYGWIN)
ENDIF(WIN32 AND CYGWIN)

## check if we are using the MINGW compiler
IF(MINGW)
  ADD_DEFINITIONS(-D__MINGW__ -D__MINGW32__ "-mms-bitfields" "-mthreads" "-Wpointer-arith" "-pipe")
  # "-fno-exceptions" can be useful too... unless you need exceptions :-)
  IF (MSYS)
    ADD_DEFINITIONS(-D__ACE_INLINE__ -DACE_HAS_ACE_TOKEN -DACE_HAS_ACE_SVCCONF -DACE_BUILD_DLL)
  ELSE (MSYS)
    ADD_DEFINITIONS("-fvisibility=hidden" "-fvisibility-inlines-hidden" "-Wno-attributes")
  ENDIF (MSYS)
ENDIF(MINGW)

FIND_PACKAGE(Ace REQUIRED)

IF (ACE_DEBUG_FOUND)
 LINK_LIBRARIES(optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY})
 SET(ACE_LIB optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY} CACHE INTERNAL "libraries")
ELSE (ACE_DEBUG_FOUND)
 LINK_LIBRARIES(${ACE_LIBRARY})
 SET(ACE_LIB ${ACE_LIBRARY} CACHE INTERNAL "libraries")
ENDIF (ACE_DEBUG_FOUND)

INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${ACE_INCLUDE_CONFIG_DIR})

INCLUDE(YarpCheckTypeSize) # regular script does not do C++ types
SET(CMAKE_EXTRA_INCLUDE_FILES ace/config.h ace/String_Base_Const.h)
SET(CMAKE_REQUIRED_INCLUDES ${ACE_INCLUDE_DIR} ${ACE_INCLUDE_CONFIG_DIR})
YARP_CHECK_TYPE_SIZE(ACE_String_Base_Const::size_type SIZE_TYPE)
SET(CMAKE_EXTRA_INCLUDE_FILES) 
SET(CMAKE_REQUIRED_INCLUDES)
SET(YARP_USE_ACE_STRING_BASE_CONST_SIZE_TYPE ${HAVE_SIZE_TYPE})




