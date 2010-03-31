# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.


#########################################################################
# Control where libraries and executables are placed

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
MESSAGE(STATUS "Libraries are placed in ${LIBRARY_OUTPUT_PATH}")
MESSAGE(STATUS "Executables are placed in ${EXECUTABLE_OUTPUT_PATH}")
# Make sure the directories actually exist
MAKE_DIRECTORY(${LIBRARY_OUTPUT_PATH})
MAKE_DIRECTORY(${EXECUTABLE_OUTPUT_PATH})
MARK_AS_ADVANCED(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
MARK_AS_ADVANCED(CMAKE_BACKWARDS_COMPATIBILITY)
IF (MSVC)
  # See the Debug/Release subdirectories - is there a more systematic
  # way to do this?
  LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib 
    ${CMAKE_BINARY_DIR}/lib/Debug 
    ${CMAKE_BINARY_DIR}/lib/Release)
ENDIF (MSVC)


#########################################################################
# Encourage user to specify build type.

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING 
	"Choose the type of build, recommanded options are: Debug or Release")
ENDIF(NOT CMAKE_BUILD_TYPE)
# Hide variable to MSVC users, since it is not needed
IF (MSVC)
  MARK_AS_ADVANCED(CMAKE_BUILD_TYPE)
ENDIF(MSVC)


#########################################################################
# Simplify compilation of portable binaries.
# To make very portable YARP binaries, put a subdirectory called
# "static_libs" in the build directory, and place any libraries
# (such as libstdc++.a) that should be linked statically there.

SET (STATLIB "${CMAKE_BINARY_DIR}/static_libs")
IF (EXISTS ${STATLIB})
    MESSAGE(STATUS "static_libs directory present: ${STATLIB}")
    LINK_DIRECTORIES(${STATLIB})
    ADD_DEFINITIONS(-static-libgcc)
    SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc")
    FILE(GLOB statlibs ${STATLIB}/*.a)
    LINK_LIBRARIES(${statlibs})
ENDIF (EXISTS ${STATLIB})


#########################################################################
# Control compilation of devices

SET(CREATE_DEVICE_LIBRARY_MODULES FALSE CACHE BOOL "Compile device modules")

IF (CREATE_DEVICE_LIBRARY_MODULES)
  SET(MERGE_DEVICE_LIBRARY_MODULES TRUE CACHE BOOL "Merge devices with YARP libraries (so they do not need to be linked separately)")
ENDIF (CREATE_DEVICE_LIBRARY_MODULES)


#########################################################################
# Control whether libraries are shared or static.
# This option isn't really usable on windows right now.

SET(CREATE_SHARED_LIBRARY FALSE CACHE BOOL "Compile shared libraries rather than linking statically")
IF (WIN32)
  MARK_AS_ADVANCED(CREATE_SHARED_LIBRARY)
ENDIF (WIN32)

IF(CREATE_SHARED_LIBRARY)
	SET(BUILD_SHARED_LIBS ON)
ENDIF(CREATE_SHARED_LIBRARY)


#########################################################################
# Control whether yarp::os::impl::String should be std::string or opaque
# Not an important option for end users yet.  In principle 
# yarp::os::ConstString could now be set to std::string, if YARP
# ever decides to accept STL as a dependency.

SET(USE_STL_STRING FALSE CACHE BOOL "Do you want the yarp String class to be std::string? (default is to use the ACE string class)")
MARK_AS_ADVANCED(USE_STL_STRING)

IF (USE_STL_STRING)
  MESSAGE(STATUS "Using std::string")
  SET(YARP_USE_STL_STRING 1)
ELSE (USE_STL_STRING)
  SET(YARP_USE_ACE_STRING 1)
ENDIF (USE_STL_STRING)


#########################################################################
# Control compilation of device tests.
# Not really for end-user, but instead for the library developers
SET(CREATE_BUILTIN_DEVICE_TESTS FALSE CACHE BOOL "Do you want to create tests for builtin devices")
MARK_AS_ADVANCED(CREATE_BUILTIN_DEVICE_TESTS CREATE_DEVICE_LIBRARY)


#########################################################################
# Defunct options to be removed

# set a flag so sub-directories know that are being compiled
# en masse as opposed to as individuals
set(COMPILING_ALL_YARP TRUE)

