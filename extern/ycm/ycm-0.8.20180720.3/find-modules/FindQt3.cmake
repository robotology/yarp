#.rst:
# FindQt3
# -------
#
# Wrap kitware's original FindQt3 script. Standardize varibles.
#
# In windows require you set QTDIR
#
# Set::
#
#  Qt3_FOUND
#  Qt3_LIBRARIES
#  Qt3_INCLUDE_DIRS
#
# .. todo:: Check if this module is still needed with recent CMake releases.
# .. todo:: Stop using Qt3 and deprecate this module


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


#message(Find Qt3 form iCub package)

# save current CMAKE_MODULE_PATH, disable it 
# to avoid recursive calls to FindGLUT
set(_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "")

if (QTDIR)
  message(${QTDIR})
endif(QTDIR)

find_package(Qt3)

if (QT_FOUND)
  set(Qt3_INCLUDE_DIRS ${QT_INCLUDE_DIR})
  set(Qt3_LIBRARIES ${QT_LIBRARIES})
  set(Qt3_FOUND TRUE)
endif(QT_FOUND)

# push back original CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${_CMAKE_MODULE_PATH})
