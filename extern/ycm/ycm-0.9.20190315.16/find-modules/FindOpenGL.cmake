#.rst:
# FindOpenGL
# ----------
#
# Wrap kitware's original FindOpenGL. Standardize variables.
#
# In windows require you set ``OpenGL_DIR``
#
# Set::
#
#  OpenGL_FOUND
#  OpenGL_LIBRARIES
#  OpenGL_INCLUDE_DIRS
#
# .. todo:: Check if this module is still needed with recent CMake releases.

#=============================================================================
# Copyright 2010 RobotCub Consortium
#   Authors: Lorenzo Natale <lorenzo.natale@iit.it>
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


# Save current CMAKE_MODULE_PATH, disable it 
# to avoid recursive calls to FindGLUT
set(_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "")

set(OpenGL_DIR $ENV{OpenGL_DIR})

if(OpenGL_DIR)
  message(${OpenGL_DIR})
endif(OpenGL_DIR)

find_package(OpenGL)

if(OPENGL_FOUND)
  set(OpenGL_FOUND TRUE)
  set(OpenGL_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
  set(OpenGL_LIBRARIES ${OPENGL_LIBRARIES})
endif(OPENGL_FOUND)

# Push back original CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${_CMAKE_MODULE_PATH})
