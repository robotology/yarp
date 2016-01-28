#.rst:
# FindGLFW3
# ---------
#
# Find the GLFW3 framework.
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``GLFW3::GLFW3``,
# if GLFW3 has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables::
#
#   GLFW3_INCLUDE_DIRS - include directories for GLFW3
#   GLFW3_LIBRARIES - libraries to link against GLFW3
#   GLFW3_FOUND - true if GLFW3 has been found and can be used
#
# Environment Variables
# ^^^^^^^^^^^^^^^^^^^^^
#
# If the library is not found on system paths, the ``GLFW3_ROOT``
# environment variable can be used to locate the lbrary.


#=============================================================================
# Copyright 2015  iCub Facility, Istituto Italiano di Tecnologia
#   Authors: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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

include(StandardFindModule)
standard_find_module(GLFW3 glfw3)

if(NOT GLFW3_FOUND)
  find_path(GLFW3_INCLUDE_DIR
            DOC "Path to GLFW3 include directory."
            NAMES GLFW/glfw3.h
            PATH_SUFFIXES include
            PATHS /usr/
                  /usr/local/
                  ${GLFW3_ROOT_DIR}
                  ENV GLFW3_ROOT)

  find_library(GLFW3_GLFW_LIBRARY
              DOC "Absolute path to GLFW3 library."
              NAMES glfw3
              PATH_SUFFIXES lib
                            lib-vc2010
              PATHS /usr/
                    /usr/local/
                    ${GLFW3_ROOT_DIR}
                    ENV GLFW3_ROOT)
  if(WIN32)
    find_library(GLFW3_OPENGL_LIBRARY
                 NAMES OpenGL32
                 PATHS "C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.0A\\Lib"
                       "C:\\Program Files (x86)\\Microsoft SDKs\\Windows\\v7.1A\\Lib")
  endif()

  set(GLFW3_GLFW_glfw3_h "${GLFW3_INCLUDE_DIR}/GLFW/glfw3.h")
  if(GLFW3_INCLUDE_DIR AND EXISTS "${GLFW3_GLFW_glfw3_h}")
    file(STRINGS "${GLFW3_GLFW_glfw3_h}" GLFW3_GLFW_glfw3_h_CONTENTS
        REGEX "^#[\t ]*define[\t ]+GLFW_VERSION_(MAJOR|MINOR|REVISION)[\t ]+[0-9]+$")

    foreach(_part MAJOR MINOR REVISION)
      string(REGEX REPLACE ".*#[\t ]*define[ \t]+GLFW_VERSION_${_part}[ \t]+([0-9]+).*" "\\1"
            GLFW3_VERSION_${_part} "${GLFW3_GLFW_glfw3_h_CONTENTS}")
    endforeach(_part)

    set(GLFW3_VERSION_STRING "${GLFW3_VERSION_MAJOR}.${GLFW3_VERSION_MINOR}.${GLFW3_VERSION_REVISION}")

  endif()

  set(GLFW3_INCLUDE_DIRS "${GLFW3_INCLUDE_DIR}")
  set(GLFW3_LIBRARIES "${GLFW3_GLFW_LIBRARY}")
  if(WIN32)
    list(APPEND GLFW3_LIBRARIES "${GLFW3_OPENGL_LIBRARY}")
  endif()

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(GLFW3
                                    REQUIRED_VARS GLFW3_LIBRARIES
                                                  GLFW3_INCLUDE_DIRS
                                    VERSION_VAR GLFW3_VERSION_STRING)
endif()

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(GLFW3 PROPERTIES DESCRIPTION "Portable library for OpenGL, window and input"
                                            URL "http://www.glfw.org/")
endif()

if(NOT GLFW3_FOUND)
  return()
endif()


# Create imported target GLFW::glfw3
add_library(GLFW3::GLFW3 STATIC IMPORTED)
set_target_properties(GLFW3::GLFW3 PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${GLFW3_INCLUDE_DIR}")

if(WIN32)
  set_target_properties(GLFW3::GLFW3 PROPERTIES
    INTERFACE_LINK_LIBRARIES "${GLFW3_OPENGL_LIBRARY}")
endif()

# Import target "GLFW3::GLFW3" for configuration "Release"
set_property(TARGET GLFW3::GLFW3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(GLFW3::GLFW3 PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${GLFW3_GLFW_LIBRARY}")
