# Copyright: (C) 2009 RobotCub Consortium
# Authors: Paul Fitzpatrick, Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


#########################################################################
# Control where libraries and executables are placed

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
message(STATUS "Libraries are placed in ${LIBRARY_OUTPUT_PATH}")
message(STATUS "Executables are placed in ${EXECUTABLE_OUTPUT_PATH}")
# Make sure the directories actually exist
make_directory(${LIBRARY_OUTPUT_PATH})
make_directory(${EXECUTABLE_OUTPUT_PATH})
mark_as_advanced(LIBRARY_OUTPUT_PATH EXECUTABLE_OUTPUT_PATH)
mark_as_advanced(CMAKE_BACKWARDS_COMPATIBILITY)
if (MSVC)
  # See the Debug/Release subdirectories - is there a more systematic
  # way to do this?  Is this still needed?
  LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib 
    ${CMAKE_BINARY_DIR}/lib/Debug 
    ${CMAKE_BINARY_DIR}/lib/Release)
endif (MSVC)


#########################################################################
# Encourage user to specify build type.

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING 
	"Choose the type of build, recommanded options are: Debug or Release")
endif(NOT CMAKE_BUILD_TYPE)
# Hide variable to MSVC users, since it is not needed
if (MSVC)
  mark_as_advanced(CMAKE_BUILD_TYPE)
endif(MSVC)


#########################################################################
# Simplify compilation of portable binaries.
# To make very portable YARP binaries, put a subdirectory called
# "static_libs" in the build directory, and place any libraries
# (such as libstdc++.a) that should be linked statically there.

set (STATLIB "${CMAKE_BINARY_DIR}/static_libs")
if (EXISTS ${STATLIB})
    MESSAGE(STATUS "static_libs directory present: ${STATLIB}")
    LINK_DIRECTORIES(${STATLIB})
    ADD_DEFINITIONS(-static-libgcc)
    SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc")
    FILE(GLOB statlibs ${STATLIB}/*.a)
    LINK_LIBRARIES(${statlibs})
endif (EXISTS ${STATLIB})


#########################################################################
# Control whether libraries are shared or static.
# This option isn't really usable on windows right now.

option(CREATE_SHARED_LIBRARY "Compile shared libraries rather than linking statically" FALSE)
if (WIN32)
  mark_as_advanced(CREATE_SHARED_LIBRARY)
endif (WIN32)

if (CREATE_SHARED_LIBRARY)
  set(BUILD_SHARED_LIBS ON)
  set(YARP_DLL ON)
endif()

#########################################################################
# Control whether yarp::os::impl::String should be std::string or opaque
# Not an important option for end users yet.  In principle 
# yarp::os::ConstString could now be set to std::string, if YARP
# ever decides to accept STL as a dependency.

option(USE_STL_STRING "Do you want the yarp String class to be std::string? (default is to use the ACE string class)" OFF)
mark_as_advanced(USE_STL_STRING)

if (USE_STL_STRING)
  message(STATUS "Using std::string")
  set(YARP_USE_STL_STRING 1)
else (USE_STL_STRING)
  set(YARP_USE_ACE_STRING 1)
endif (USE_STL_STRING)


#########################################################################
# Control compilation of device tests.
# Not really for end-user, but instead for the library developers
set(CREATE_BUILTIN_DEVICE_TESTS FALSE CACHE BOOL "Do you want to create tests for builtin devices")
mark_as_advanced(CREATE_BUILTIN_DEVICE_TESTS)


#########################################################################
# Control submission of reports
option(ENABLE_DASHBOARD_SUBMIT "Allow submission of builds to http://my.cdash.org/index.php?project=YARP" OFF)
if (ENABLE_DASHBOARD_SUBMIT)
  include(Dart)
endif (ENABLE_DASHBOARD_SUBMIT)


#########################################################################
# Control setting an rpath
if (NOT MSVC)
  option(INSTALL_WITH_RPATH "When installing YARP, give executables a full RPATH" FALSE)
  option(ENABLE_FORCE_RPATH "Set an rpath after installing the executables (deprecated, please use INSTALL_WITH_RPATH" FALSE)
  mark_as_advanced(ENABLE_FORCE_RPATH)
endif (NOT MSVC)

if (INSTALL_WITH_RPATH OR ENABLE_FORCE_RPATH)
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()

#########################################################################
# Defunct options to be removed

# set a flag so sub-directories know that are being compiled
# en masse as opposed to as individuals
set(COMPILING_ALL_YARP TRUE)




