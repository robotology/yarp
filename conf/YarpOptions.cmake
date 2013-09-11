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

set(YARP_OPTIMIZED_CONFIGURATIONS "Release" "MinSizeRel")
set(YARP_DEBUG_CONFIGURATIONS "Debug" "RelWithDebInfo")

#########################################################################
# DebugFull builds options

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS_DEBUGFULL "-O0 -g3 -ggdb -fno-inline -DDEBUG" CACHE STRING "Flags used by the compiler during debugfull builds.")
    set(CMAKE_CXX_FLAGS_DEBUGFULL "-O0 -g3 -ggdb -fno-inline -DDEBUG" CACHE STRING "Flags used by the compiler during debugfull builds.")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUGFULL "" CACHE STRING "Flags used by the linker during debugfull builds.")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUGFULL "" CACHE STRING "Flags used by the linker during debugfull builds.")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUGFULL "" CACHE STRING "Flags used by the linker during debugfull builds.")

    mark_as_advanced(CMAKE_C_FLAGS_DEBUGFULL
                     CMAKE_CXX_FLAGS_DEBUGFULL
                     CMAKE_EXE_LINKER_FLAGS_DEBUGFULL
                     CMAKE_MODULE_LINKER_FLAGS_DEBUGFULL
                     CMAKE_SHARED_LINKER_FLAGS_DEBUGFULL)

    list(APPEND YARP_DEBUG_CONFIGURATIONS "DebugFull")
endif(CMAKE_COMPILER_IS_GNUCXX)

#########################################################################
# Profile builds options

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS_PROFILE "-pg -g3 -ggdb -fno-inline -ftest-coverage -fprofile-arcs -DNDEBUG" CACHE STRING "Flags used by the compiler during profile builds.")
    set(CMAKE_CXX_FLAGS_PROFILE "-pg -g3 -ggdb -fno-inline -ftest-coverage -fprofile-arcs -DNDEBUG" CACHE STRING "Flags used by the compiler during profile builds.")
    set(CMAKE_EXE_LINKER_FLAGS_PROFILE "" CACHE STRING "Flags used by the linker during profile builds.")
    set(CMAKE_MODULE_LINKER_FLAGS_PROFILE "" CACHE STRING "Flags used by the linker during profile builds.")
    set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "" CACHE STRING "Flags used by the linker during profile builds.")

    mark_as_advanced(CMAKE_C_FLAGS_PROFILE
                     CMAKE_CXX_FLAGS_PROFILE
                     CMAKE_EXE_LINKER_FLAGS_PROFILE
                     CMAKE_MODULE_LINKER_FLAGS_PROFILE
                     CMAKE_SHARED_LINKER_FLAGS_PROFILE)

    list(APPEND YARP_DEBUG_CONFIGURATIONS "Profile")
endif(CMAKE_COMPILER_IS_GNUCXX)

#########################################################################
# Handle CMAKE_CONFIGURATION_TYPES and CMAKE_BUILD_TYPE

if(NOT CMAKE_CONFIGURATION_TYPES)
    # Possible values for the CMAKE_BUILD_TYPE variable
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS ${YARP_OPTIMIZED_CONFIGURATIONS} ${YARP_DEBUG_CONFIGURATIONS})
    if(NOT CMAKE_BUILD_TYPE)
        # Encourage user to specify build type.
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
    endif()
endif()

# Let CMake know which configurations are the debug ones, so that it can
# link the right library when both optimized and debug library are found
set_property(GLOBAL PROPERTY DEBUG_CONFIGURATIONS ${YARP_DEBUG_CONFIGURATIONS})


#########################################################################
# Simplify compilation of portable binaries.
# To make very portable YARP binaries, put a subdirectory called
# "static_libs" in the build directory, and place any libraries
# (such as libstdc++.a) that should be linked statically there.

set(STATLIB "${CMAKE_BINARY_DIR}/static_libs")
if(EXISTS ${STATLIB})
    message(STATUS "static_libs directory present: ${STATLIB}")
    link_directories(${STATLIB})
    add_definitions(-static-libgcc)
    set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc")
    file(GLOB statlibs ${STATLIB}/*.a)
    link_libraries(${statlibs})
endif(EXISTS ${STATLIB})


#########################################################################
# Control whether libraries are shared or static.

option(CREATE_SHARED_LIBRARY "Compile shared libraries rather than linking statically" FALSE)
if (WIN32)
  set(YARP_FILTER_API TRUE)
endif (WIN32)

if (CREATE_SHARED_LIBRARY)
  set(BUILD_SHARED_LIBS ON)
  set(YARP_DLL ON)
endif()

#########################################################################
# Enable these messages for debugging flags

#message(STATUS "WANTED_WARNING_FLAGS = ${WANTED_WARNING_FLAGS}")
#message(STATUS "EXPERIMENTAL_WARNING_FLAGS = ${EXPERIMENTAL_WARNING_FLAGS}")
#message(STATUS "UNWANTED_WARNING_FLAGS = ${UNWANTED_WARNING_FLAGS}")
#message(STATUS "VISIBILITY_HIDDEN_FLAGS = ${VISIBILITY_HIDDEN_FLAGS}")
#message(STATUS "DEPRECATED_DECLARATIONS_FLAGS = ${DEPRECATED_DECLARATIONS_FLAGS}")
#message(STATUS "HARDENING_FLAGS = ${HARDENING_FLAGS}")
#message(STATUS "CXX11_FLAGS = ${CXX11_FLAGS}")


#########################################################################
# Enable/disable wanted, experimental, and unwanted warnings

# FIXME We should fix those warnings and move them to wanted
option(YARP_EXPERIMENTAL_WARNINGS "Enable some more checks that at the moment produce a lot of warnings" FALSE)
mark_as_advanced(YARP_EXPERIMENTAL_WARNINGS)
if(YARP_EXPERIMENTAL_WARNINGS)
    set(WANTED_WARNING_FLAGS "${WANTED_WARNING_FLAGS} ${EXPERIMENTAL_WARNING_FLAGS}")
endif(YARP_EXPERIMENTAL_WARNINGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WANTED_WARNING_FLAGS} ${UNWANTED_WARNING_FLAGS}")


#########################################################################
# Control whether non-public symbols are filtered out

option(YARP_CLEAN_API "Filter out non-public symbols" FALSE)
mark_as_advanced(YARP_CLEAN_API)
if(YARP_CLEAN_API)
    if(YARP_COMPILE_TESTS)
        if(WIN32)
            message(FATAL_ERROR "On Windows, we cannot compile tests when building dlls. Turn one of YARP_COMPILE_TESTS or CREATE_SHARED_LIBRARY off.")
        else(WIN32)
            message(STATUS "Since tests access non-public classes, we'll need to leave all symbols in the shared libraries. If this is undesired, turn one of YARP_COMPILE_TESTS or CREATE_SHARED_LIBRARY off.")
        endif(WIN32)
    else(YARP_COMPILE_TESTS)
        set(YARP_FILTER_API TRUE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VISIBILITY_HIDDEN_FLAGS}")
    endif(YARP_COMPILE_TESTS)
endif(YARP_CLEAN_API)


#########################################################################
# Show warnings for deprecated declarations

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEPRECATED_DECLARATIONS_FLAGS}")


#########################################################################
# Control whether deprecated methods are built and whether they should
# print warnings

option(YARP_NO_DEPRECATED "Filter out deprecated declarations from API" FALSE)
mark_as_advanced(YARP_NO_DEPRECATED)
if(YARP_NO_DEPRECATED)
    add_definitions("-DYARP_NO_DEPRECATED")
endif(YARP_NO_DEPRECATED)

include(CMakeDependentOption)
cmake_dependent_option(YARP_NO_DEPRECATED_WARNINGS
                       "Filter out deprecated declarations from API" FALSE
                       "NOT YARP_NO_DEPRECATED" FALSE)
mark_as_advanced(YARP_NO_DEPRECATED_WARNINGS)
if(YARP_NO_DEPRECATED_WARNINGS)
    add_definitions("-DYARP_NO_DEPRECATED_WARNINGS")
endif(YARP_NO_DEPRECATED_WARNINGS)

#########################################################################
# Control whether to build YARP using hardening options

option(YARP_EXPERIMENTAL_HARDENING "Build YARP using hardening flags" FALSE)
mark_as_advanced(YARP_EXPERIMENTAL_HARDENING)
if(YARP_EXPERIMENTAL_HARDENING)
    add_definitions("-D_FORTIFY_SOURCE=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HARDENING_FLAGS}")
endif(YARP_EXPERIMENTAL_HARDENING)


#########################################################################
# Control whether to build YARP using hardening options

option(YARP_EXPERIMENTAL_CXX11 "Build YARP using C++11 standard" FALSE)
mark_as_advanced(YARP_EXPERIMENTAL_CXX11)
if(YARP_EXPERIMENTAL_CXX11)
   add_definitions("-DYARP_CXX11")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAGS}")
   set(YARP_HAS_CXX11 TRUE)
endif(YARP_EXPERIMENTAL_CXX11)


#########################################################################
# Control whether yarp::os::impl::String should be std::string or opaque
# Not an important option for end users yet.  In principle
# yarp::os::ConstString could now be set to std::string, if YARP
# ever decides to accept STL as a dependency.

set (YARP_WRAP_STL_STRING_DEFAULT FALSE)
if (MSVC)
  set (YARP_WRAP_STL_STRING_DEFAULT TRUE)
endif ()
option(YARP_WRAP_STL_STRING "Do you want the yarp string classes to wrap std::string? (as opposed to being exactly std::string)" ${YARP_WRAP_STL_STRING_DEFAULT})
mark_as_advanced(YARP_WRAP_STL_STRING)

#########################################################################
# Control compilation of device tests.
# Not really for end-user, but instead for the library developers
option(CREATE_BUILTIN_DEVICE_TESTS "Do you want to create tests for builtin devices" FALSE)
mark_as_advanced(CREATE_BUILTIN_DEVICE_TESTS)


#########################################################################
# Control submission of reports
option(ENABLE_DASHBOARD_SUBMIT "Allow submission of builds to http://dashboard.icub.org/index.php?project=YARP" OFF)
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
  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif ()


#########################################################################
# Control whether build should be aborted on warnings and errors


# TODO Remove this later
set(YARP_ADMIN "$ENV{YARP_ADMIN}")

if(YARP_ADMIN)
    message("YARP_ADMIN environment variable is deprecated. Consider setting YARP_ABORT_ON_WARNING and/or YARP_FATAL_ERRORS instead")
endif(YARP_ADMIN)


### -Werror
if(CXX_HAS_WERROR)
    option(YARP_ABORT_ON_WARNING "Consider compiler warnings as errors and abort compilation (-Werror)." TRUE)
    mark_as_advanced(YARP_ABORT_ON_WARNING)
    if(YARP_ABORT_ON_WARNING)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
     endif(YARP_ABORT_ON_WARNING)
endif(CXX_HAS_WERROR)


### -Wfatal-errors
if(CXX_HAS_WFATAL_ERROR)
    option(YARP_FATAL_ERRORS "Abort compilation on the first error rather than trying to keep going and printing further error messages (-Wfatal-errors)" FALSE)
    mark_as_advanced(YARP_FATAL_ERRORS)
    if(YARP_FATAL_ERRORS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")
    endif(YARP_FATAL_ERRORS)
endif(CXX_HAS_WFATAL_ERROR)
