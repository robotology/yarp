# This is a collection of workarounds that are used in yarp to support
# older versions of CMake

# Copyright (C) 2013  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# Define visual studio version
if(CMAKE_VERSION VERSION_LESS 2.8.10 AND MSVC)
    if(MSVC_VERSION EQUAL 1200)
        set(MSVC60 TRUE)
    elseif(MSVC_VERSION EQUAL 1300)
        set(MSVC70 TRUE)
    elseif(MSVC_VERSION EQUAL 1310)
        set(MSVC71 TRUE)
#for some reason far beyond my knowledge MSVC80 is defined since 2.6
#    elseif(MSVC_VERSION EQUAL 1400)
#        set(MSVC80 TRUE)
    elseif(MSVC_VERSION EQUAL 1500)
        set(MSVC90 TRUE)
    elseif(MSVC_VERSION EQUAL 1600)
        set(MSVC10 TRUE)
    elseif(MSVC_VERSION EQUAL 1700)
        set(MSVC11 TRUE)
    endif()
endif()

# CMake 2.8.11 fails on some systems with a very obscure error.
# This is due to a bug when quoting libraries in try_compile, and it was
# fixed in 2.8.11.1. Older versions do not seem to be this problem.
# In order to ensure that the user do not use the bugged version (and
# consequently complain about not being able to build the project) we
# ensure here that the version is different from 2.8.11 and print a
# readable error that the user will be able to understand.
#
# See also: http://cmake.org/gitweb?p=cmake.git;a=commit;h=e65ef08bf2719ffd1cc4226f9594ff7127ad8b5e
if(CMAKE_VERSION VERSION_EQUAL 2.8.11)
    message(FATAL_ERROR "CMake 2.8.11 is bugged and the build will fail. Please install a different version.")
endif()
