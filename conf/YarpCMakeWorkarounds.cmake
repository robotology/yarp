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
#for some reason far behond my knowledge MSVC80 is defined since 2.6
#    elseif(MSVC_VERSION EQUAL 1400)
#        set(MSVC80 TRUE)
    elseif(MSVC_VERSION EQUAL 1500)
        set(MSVC90 TRUE)
    elseif(MSVC_VERSION EQUAL 1600)
        set(MSVC10 TRUE)
    elseif(MSVC_VERSION EQUAL 1700)
        set(MSVC10 TRUE)
    endif()
endif()
