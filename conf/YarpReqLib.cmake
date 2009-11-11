
INCLUDE(UsePkgConfig)

SET(YARP_ADMIN "$ENV{YARP_ADMIN}")

IF (YARP_ADMIN)
  # be very serious about warnings if in admin mode
  ADD_DEFINITIONS(-Werror -Wfatal-errors)
ENDIF (YARP_ADMIN)

SET(YARP_DEFINES_ACCUM "-DYARP_PRESENT")
SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} "-D_REENTRANT")

# on windows, we have to tell ace how it was compiled
IF (WIN32 AND NOT CYGWIN)
  SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DWIN32 -D_WINDOWS)
ELSE (WIN32 AND NOT CYGWIN)
  # flush out warnings for those who want to see everything.
  # this used to be conditional on YARP_ADMIN, but really no good
  # can come of suppressing these messages --paulfitz
  ADD_DEFINITIONS(-Wall)
  #SET(YARP_ADMIN "$ENV{YARP_ADMIN}")
  #IF (YARP_ADMIN)
  #ENDIF (YARP_ADMIN)
  # don't add to YARP_DEFINES_ACCUM since clients may not want it
ENDIF (WIN32 AND NOT CYGWIN)

## check if we are on cygwin
IF(WIN32 AND CYGWIN)
  SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DCYGWIN)
ENDIF(WIN32 AND CYGWIN)

## check if we are using the MINGW compiler
IF(MINGW)
  SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -D__MINGW__ -D__MINGW32__ "-mms-bitfields" "-mthreads" "-Wpointer-arith" "-pipe")
  # "-fno-exceptions" can be useful too... unless you need exceptions :-)
  IF (MSYS)
    SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -D__ACE_INLINE__ -DACE_HAS_ACE_TOKEN -DACE_HAS_ACE_SVCCONF -DACE_BUILD_DLL)
  ELSE (MSYS)
    SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} "-fvisibility=hidden" "-fvisibility-inlines-hidden" "-Wno-attributes")
  ENDIF (MSYS)
ENDIF(MINGW)

# check endianness
IF(EXISTS "${CMAKE_ROOT}/Modules/TestBigEndian.cmake")
    INCLUDE(TestBigEndian)
    TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
    IF(${IS_BIG_ENDIAN})
	SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_BIG_ENDIAN)
    ELSE(${IS_BIG_ENDIAN})
	SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_LITTLE_ENDIAN)
    ENDIF(${IS_BIG_ENDIAN})
ENDIF(EXISTS "${CMAKE_ROOT}/Modules/TestBigEndian.cmake")

# check if we want yarp String to be std::string
IF (USE_STL_STRING)
  SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_USE_STL_STRING=1)
  MESSAGE(STATUS "Using std::string")
ENDIF (USE_STL_STRING)



# get an int32 type
IF(EXISTS "${CMAKE_ROOT}/Modules/CheckTypeSize.cmake")
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
        # well, we are in trouble - there's no other native type to get 16 bits
	MESSAGE(STATUS "Warning: cannot find a 16 bit type on your system")
	MESSAGE(STATUS "Continuing...")
    ENDIF(SIZEOF_SHORT EQUAL 2)

    IF(YARP_INT32)
        SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_INT32=${YARP_INT32})
    ENDIF(YARP_INT32)

    IF(YARP_INT16)
        SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_INT16=${YARP_INT16})
    ENDIF(YARP_INT16)

    CHECK_TYPE_SIZE("double" SIZEOF_DOUBLE)
    IF(SIZEOF_DOUBLE EQUAL 8)
        SET(YARP_FLOAT64 "double")
    ELSE(SIZEOF_DOUBLE EQUAL 8)
        IF(SIZEOF_FLOAT EQUAL 8)
            SET(YARP_FLOAT64 "float")
        ENDIF(SIZEOF_FLOAT EQUAL 8)
    ENDIF(SIZEOF_DOUBLE EQUAL 8)

    IF(YARP_FLOAT64)
        SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_FLOAT64=${YARP_FLOAT64})
    ENDIF(YARP_FLOAT64)

ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CheckTypeSize.cmake")

FIND_PACKAGE(Ace REQUIRED)

IF (CREATE_GUIS)
  FIND_PACKAGE(GtkPlus)
  IF (NOT GtkPlus_FOUND)
	MESSAGE(STATUS " gtk+ not found, won't compile dependent tools")
  ENDIF (NOT GtkPlus_FOUND)

  FIND_PACKAGE(Gthread)
  IF (NOT Gthread_FOUND)
	MESSAGE(STATUS " gthread not found, won't compile dependent tools")
  ENDIF(NOT Gthread_FOUND)
# GtkMM dependencies moving to iCub
#  FIND_PACKAGE(GtkMM)
#  IF(NOT GtkMM_FOUND)
#        MESSAGE(STATUS " gtkmm not found, won't compile dependent tools")
#  ENDIF(NOT GtkMM_FOUND)
ENDIF (CREATE_GUIS)

#MESSAGE(STATUS "defs ${YARP_DEFINES_ACCUM}")

SET(YARP_HAS_MATH_LIB  FALSE)

IF (CREATE_LIB_MATH)
FIND_PACKAGE(GSL REQUIRED)
IF (NOT GSL_FOUND)
	MESSAGE(STATUS "GSL not found, won't compile libYARP_math")
ELSE (NOT GSL_FOUND)
	SET(YARP_HAS_MATH_LIB TRUE)
ENDIF (NOT GSL_FOUND)
ENDIF(CREATE_LIB_MATH)

IF (ACE_DEBUG_FOUND)
 LINK_LIBRARIES(optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY})
 SET(ACE_LIB optimized ${ACE_LIBRARY} debug ${ACE_DEBUG_LIBRARY} CACHE INTERNAL "libraries")
ELSE (ACE_DEBUG_FOUND)
 LINK_LIBRARIES(${ACE_LIBRARY})
 SET(ACE_LIB ${ACE_LIBRARY} CACHE INTERNAL "libraries")
ENDIF (ACE_DEBUG_FOUND)

INCLUDE_DIRECTORIES(${ACE_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${ACE_INCLUDE_CONFIG_DIR})

IF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)
  IF(EXISTS "${CMAKE_ROOT}/Modules/CheckTypeSize.cmake")
    INCLUDE(YarpCheckTypeSize) # regular script does not do C++ types
    SET(CMAKE_EXTRA_INCLUDE_FILES ace/config.h ace/String_Base_Const.h)
    YARP_CHECK_TYPE_SIZE(ACE_String_Base_Const::size_type SIZE_TYPE)
    SET(CMAKE_EXTRA_INCLUDE_FILES) 
  ENDIF(EXISTS "${CMAKE_ROOT}/Modules/CheckTypeSize.cmake")
ENDIF (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 2.5)

IF (HAVE_SIZE_TYPE)
  SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_ACE_HAS_SIZE_TYPE)
ELSE (HAVE_SIZE_TYPE)
  SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_ACE_HAS_NO_SIZE_TYPE)
ENDIF (HAVE_SIZE_TYPE)

SET(YARP_DEFINES_ACCUM ${YARP_DEFINES_ACCUM} -DYARP_VERSION=${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})


# give a readout of defines
SET(YARP_DEFINES ${YARP_DEFINES_ACCUM} CACHE STRING "Definitions needed when compiling with YARP")
MARK_AS_ADVANCED(YARP_DEFINES)

ADD_DEFINITIONS(${YARP_DEFINES_ACCUM})


# make this visual studio specific, otherwise see warnings on e.g. OSX
# that these directories don't exist
IF (MSVC)
  # msvc problems...
  LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib ${CMAKE_BINARY_DIR}/lib/Debug ${CMAKE_BINARY_DIR}/lib/Release)
ENDIF (MSVC)


# acan't do this on windows without a lot of mess
IF(CREATE_SHARED_LIBRARY)
	SET(BUILD_SHARED_LIBS ON)
ENDIF(CREATE_SHARED_LIBRARY)


