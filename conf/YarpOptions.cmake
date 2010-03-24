# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the GNU GPL v2.0.


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
# Control compilation of GUI code (now just yarpview, other parts
# were moved to iCub project)

SET(CREATE_GUIS FALSE CACHE BOOL "Do you want to compile GUIs")


#########################################################################
# Control compilation of math library

SET(CREATE_LIB_MATH FALSE CACHE BOOL "Create math library libYARP_math")


#########################################################################
# Control whether libraries are shared or static.
# This option isn't really usable on windows right now.

SET(CREATE_SHARED_LIBRARY FALSE CACHE BOOL "Compile shared libraries rather than linking statically")
IF (WIN32)
  MARK_AS_ADVANCED(CREATE_SHARED_LIBRARY)
ENDIF (WIN32)


#########################################################################
# Control whether yarp::os::impl::String should be std::string or opaque
# Not an important option for end users yet.  In principle 
# yarp::os::ConstString could now be set to std::string, if YARP
# ever decides to accept STL as a dependency.

SET(USE_STL_STRING FALSE CACHE BOOL "Do you want the yarp String class to be std::string? (default is to use the ACE string class)")
MARK_AS_ADVANCED(USE_STL_STRING)


#########################################################################
# Control compilation of device tests.
# Not really for end-user, but instead for the library developers
SET(CREATE_BUILTIN_DEVICE_TESTS FALSE CACHE BOOL "Do you want to create tests for builtin devices")
MARK_AS_ADVANCED(CREATE_BUILTIN_DEVICE_TESTS CREATE_DEVICE_LIBRARY)


#########################################################################
# Defunct options to be removed

SET(COMPILE_NEW_YARPVIEW TRUE CACHE BOOL "Do you want to compile the new yarpview?")
MARK_AS_ADVANCED(COMPILE_NEW_YARPVIEW)

# set a flag so sub-directories know that are being compiled
# en masse as opposed to as individuals
set(COMPILING_ALL_YARP TRUE)

