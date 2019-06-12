#=============================================================================
# Copyright 2019 Istituto Italiano di Tecnologia (IIT)
#   Authors: Claudio Fantacci <claudio.fantacci@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

#[=======================================================================[.rst:
FindGLM
-------

Find the OpenGL Mathematics (glm)

Input variables
^^^^^^^^^^^^^^^

The following variables may be set to influence this module’s behavior:

``GLM_VERBOSE``
  to output a detailed log of this module.

Imported Targets
^^^^^^^^^^^^^^^^

This module defines the :prop_tgt:`IMPORTED` target ``glm`` for the
library glm.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``GLM_FOUND``
  true if assimp has been found and can be used
``GLM_INCLUDE_DIRS``
  include directories for assimp
``GLM_VERSION``
  GLM version
``GLM_VERSION_MAJOR``
  GLM major version
``GLM_VERSION_MINOR``
  GLM minor version
``GLM_VERSION_PATCH``
  GLM patch version

#]=======================================================================]


include(FindPackageHandleStandardArgs)

find_package(GLM CONFIG QUIET)

if(GLM_FOUND)
  find_package_handle_standard_args(GLM DEFAULT_MSG GLM_CONFIG)
  return()
endif()

if(GLM_VERBOSE)
  message(STATUS "Findglm: did not find glm CMake config file. Searching for header files.")
endif()


find_path(GLM_INCLUDE_DIR glm/glm.hpp)
mark_as_advanced(GLM_INCLUDE_DIR)

set(GLM_INCLUDE_DIRS ${GLM_INCLUDE_DIR})

if(GLM_VERBOSE)
  message(STATUS "Findglm: GLM_INCLUDE_DIR: ${GLM_INCLUDE_DIR}")
  message(STATUS "Findglm: GLM_INCLUDE_DIRS: ${GLM_INCLUDE_DIRS}")
endif()

if(EXISTS "${GLM_INCLUDE_DIR}/glm/detail/setup.hpp")
  file(STRINGS "${GLM_INCLUDE_DIR}/glm/detail/setup.hpp" _contents REGEX "#define GLM_VERSION_.+[ \t]+[0-9]+")
  if(_contents)
    string(REGEX REPLACE ".*VERSION_MAJOR[ \t]+([0-9]+).*" "\\1" GLM_VERSION_MAJOR "${_contents}")
    string(REGEX REPLACE ".*VERSION_MINOR[ \t]+([0-9]+).*" "\\1" GLM_VERSION_MINOR "${_contents}")
    string(REGEX REPLACE ".*VERSION_PATCH[ \t]+([0-9]+).*" "\\1" GLM_VERSION_PATCH "${_contents}")
    set(GLM_VERSION "${GLM_VERSION_MAJOR}.${GLM_VERSION_MINOR}.${GLM_VERSION_PATCH}")
  endif()
endif()

if(GLM_VERBOSE)
  message(STATUS "Findglm: GLM_VERSION_MAJOR: ${GLM_VERSION_MAJOR}")
  message(STATUS "Findglm: GLM_VERSION_MINOR: ${GLM_VERSION_MINOR}")
  message(STATUS "Findglm: GLM_VERSION_PATCH: ${GLM_VERSION_PATCH}")
  message(STATUS "Findglm: GLM_VERSION: ${GLM_VERSION}")
endif()

find_package_handle_standard_args(GLM
                                  REQUIRED_VARS GLM_INCLUDE_DIRS
                                  VERSION_VAR GLM_VERSION)

if(NOT GLM_FOUND)
  if(GLM_VERBOSE)
    message(STATUS "Findglm: could not found glm headers.")
  endif()
  return()
endif()


if(NOT TARGET glm)
  if(GLM_VERBOSE)
    message(STATUS "Findglm: Creating glm imported target.")
  endif()

  add_library(glm INTERFACE IMPORTED)

  set_target_properties(glm
                        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${GLM_INCLUDE_DIRS}")

endif()

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(assimp PROPERTIES DESCRIPTION "OpenGL Mathematics (GLM)"
                                             URL "https://glm.g-truc.net/")
endif()
