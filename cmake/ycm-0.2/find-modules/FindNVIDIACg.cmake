# Copyright: (C) 2009 RobotCub Consortium
#                2016 Istituto Italiano di Tecnologia, iCub Facility
# Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
#          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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
