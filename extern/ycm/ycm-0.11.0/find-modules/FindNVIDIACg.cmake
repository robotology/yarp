#.rst:
# FindNVIDIACg
# ------------
#
# Try to find NVIDIACg libraries

#=============================================================================
# Copyright 2009 RobotCub Consortium
# Copyright 2016 Istituto Italiano di Tecnologia (IIT)
#   Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
#            Daniele E. Domenichelli <daniele.domenichelli@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)

find_library(NVIDIACg_CgGL_LIBRARY CgGL)
find_library(NVIDIACg_pthread_LIBRARY pthread)
find_library(NVIDIACg_GL_LIBRARY GL)
find_library(NVIDIACg_glut_LIBRARY glut)
if(NOT WIN32)
  find_library(NVIDIACg_GLEW_LIBRARY GLEW)
endif()

set(NVIDIACg_LIBRARIES ${NVIDIACg_pthread_LIBRARY}
                       ${NVIDIACg_GL_LIBRARY}
                       ${NVIDIACg_glut_LIBRARY}
                       ${NVIDIACg_CgGL_LIBRARY})
if(NOT WIN32)
  list(APPEND NVIDIACg_LIBRARIES ${NVIDIACg_GLEW_LIBRARY})
endif()

find_path(NVIDIACg_INCLUDE_DIR "Cg/cg.h")
set(NVIDIACg_INCLUDE_DIRS ${NVIDIACg_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NVIDIACg DEFAULT_MSG NVIDIACg_LIBRARIES NVIDIACg_INCLUDE_DIRS)
