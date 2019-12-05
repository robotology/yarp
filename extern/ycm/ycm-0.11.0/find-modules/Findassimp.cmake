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
Findassimp
----------

Find the Open Asset Importer Library (assimp)

Input variables
^^^^^^^^^^^^^^^

The following variables may be set to influence this module’s behavior:

``ASSIMP_VERBOSE``
  to output a detailed log of this module.

Imported Targets
^^^^^^^^^^^^^^^^

This module defines the :prop_tgt:`IMPORTED` target ``assimp::assimp`` for the
library assimp.

Result Variables
^^^^^^^^^^^^^^^^

This module defines the following variables:

``assimp_FOUND``
  true if assimp has been found and can be used
``ASSIMP_ROOT_DIR``
  the root directory where the installation can be found
``ASSIMP_INCLUDE_DIRS``
  include directories for assimp
``ASSIMP_LIBRARIES``
  libraries to link against assimp
``ASSIMP_LIBRARY_DIRS``
  link directories
``ASSIMP_VERSION``
  ASSIMP version
``ASSIMP_VERSION_MAJOR``
  ASSIMP major version
``ASSIMP_VERSION_MINOR``
  ASSIMP minor version
``ASSIMP_VERSION_REVISION``
  ASSIMP revision version

#]=======================================================================]

set(ASSIMP_SHARED_LIBRARY_NAME)
set(ASSIMP_SHARED_LIBRARY_NAME_DEBUG)

set(ASSIMP_STATIC_LIBRARY_NAME)
set(ASSIMP_STATIC_LIBRARY_NAME_DEBUG)

if(MSVC)
  if(MSVC70 OR MSVC71)
    set(MSVC_PREFIX "vc70")
  elseif(MSVC80)
    set(MSVC_PREFIX "vc80")
  elseif(MSVC90)
    set(MSVC_PREFIX "vc90")
  elseif(MSVC10)
    set(MSVC_PREFIX "vc100")
  elseif(MSVC11)
    set(MSVC_PREFIX "vc110")
  elseif(MSVC12)
    set(MSVC_PREFIX "vc120")
  elseif(MSVC14)
    set(MSVC_PREFIX "vc140")
  else()
    set(MSVC_PREFIX "vc150")
  endif()

  set(ASSIMP_LIBRARY_SUFFIX "-${MSVC_PREFIX}-mt")

  set(ASSIMP_SHARED_LIBRARY_NAME "assimp${ASSIMP_LIBRARY_SUFFIX}")
  set(ASSIMP_SHARED_LIBRARY_NAME_DEBUG "assimp${ASSIMP_LIBRARY_SUFFIX}d")

  set(ASSIMP_STATIC_LIBRARY_NAME "assimp${ASSIMP_LIBRARY_SUFFIX}s")
  set(ASSIMP_STATIC_LIBRARY_NAME_DEBUG "assimp${ASSIMP_LIBRARY_SUFFIX}ds")


  if(ASSIMP_VERBOSE)
    message(STATUS "Findassimp: ASSIMP_LIBRARY_SUFFIX: ${ASSIMP_LIBRARY_SUFFIX}")
    message(STATUS "Findassimp: ASSIMP_SHARED_LIBRARY_NAME: ${ASSIMP_SHARED_LIBRARY_NAME}")
    message(STATUS "Findassimp: ASSIMP_SHARED_LIBRARY_NAME_DEBUG: ${ASSIMP_SHARED_LIBRARY_NAME_DEBUG}")
    message(STATUS "Findassimp: ASSIMP_STATIC_LIBRARY_NAME: ${ASSIMP_STATIC_LIBRARY_NAME}")
    message(STATUS "Findassimp: ASSIMP_STATIC_LIBRARY_NAME_DEBUG: ${ASSIMP_STATIC_LIBRARY_NAME_DEBUG}")
  endif()
endif()


# Include directories
find_path(ASSIMP_INCLUDE_DIR assimp/scene.h)
mark_as_advanced(ASSIMP_INCLUDE_DIR)

set(ASSIMP_INCLUDE_DIRS ${ASSIMP_INCLUDE_DIR})

get_filename_component(ASSIMP_ROOT_DIR ${ASSIMP_INCLUDE_DIR} DIRECTORY)

if(ASSIMP_VERBOSE)
  message(STATUS "Findassimp: ASSIMP_INCLUDE_DIR: ${ASSIMP_INCLUDE_DIR}")
  message(STATUS "Findassimp: ASSIMP_INCLUDE_DIRS: ${ASSIMP_INCLUDE_DIRS}")
  message(STATUS "Findassimp: ASSIMP_ROOT_DIR: ${ASSIMP_ROOT_DIR}")
endif()


find_library(ASSIMP_LIBRARY_RELEASE
             NAMES assimp ${ASSIMP_SHARED_LIBRARY_NAME} ${ASSIMP_STATIC_LIBRARY_NAME}
             PATH_SUFFIXES lib lib64 libx32
             PATHS ENV ASSIMP_ROOT)

find_library(ASSIMP_LIBRARY_DEBUG
             NAMES ${ASSIMP_SHARED_LIBRARY_NAME_DEBUG} ${ASSIMP_STATIC_LIBRARY_NAME_DEBUG}
             PATH_SUFFIXES lib lib64
             PATHS ENV ASSIMP_ROOT)


include(SelectLibraryConfigurations)

select_library_configurations(ASSIMP)

if(ASSIMP_LIBRARY)
  set(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY})

  get_filename_component(ASSIMP_LIBRARY_DIRS_RELEASE ${ASSIMP_LIBRARY_RELEASE} DIRECTORY)
  get_filename_component(ASSIMP_LIBRARY_DIRS_DEBUG ${ASSIMP_LIBRARY_DEBUG} DIRECTORY)

  list(APPEND ASSIMP_LIBRARY_DIRS ${ASSIMP_LIBRARY_DIRS_RELEASE} ${ASSIMP_LIBRARY_DIRS_DEBUG})

else()
  set(ASSIMP_LIBRARIES NOTFOUND)
  set(ASSIMP_LIBRARY_DIRS NOTFOUND)
endif()


if(ASSIMP_VERBOSE)
  message(STATUS "Findassimp: ASSIMP_LIBRARY_RELEASE: ${ASSIMP_LIBRARY_RELEASE}")
  message(STATUS "Findassimp: ASSIMP_LIBRARY_DEBUG: ${ASSIMP_LIBRARY_DEBUG}")
  message(STATUS "Findassimp: ASSIMP_LIBRARY: ${ASSIMP_LIBRARY}")
  message(STATUS "Findassimp: ASSIMP_LIBRARIES: ${ASSIMP_LIBRARIES}")
  message(STATUS "Findassimp: ASSIMP_LIBRARY_DIRS: ${ASSIMP_LIBRARY_DIRS}")
endif()


if(NOT ASSIMP_INCLUDE_DIRS OR NOT ASSIMP_LIBRARIES)
  if(ASSIMP_VERBOSE)
    message(STATUS "Findassimp: could not found assimp library.")
  endif()
  return()
endif()


# Create the target
if(NOT TARGET assimp::assimp)
  if(ASSIMP_VERBOSE)
    message(STATUS "Findassimp: Creating assimp::assimp imported target.")
  endif()

  add_library(assimp::assimp UNKNOWN IMPORTED)

  set_target_properties(assimp::assimp
                        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ASSIMP_INCLUDE_DIRS}")

  if(ASSIMP_LIBRARY_RELEASE)
    set_property(TARGET assimp::assimp
                 APPEND
                 PROPERTY IMPORTED_CONFIGURATIONS RELEASE)


    set_target_properties(assimp::assimp
                          PROPERTIES IMPORTED_LOCATION_RELEASE "${ASSIMP_LIBRARY_RELEASE}")
  endif()

  if(ASSIMP_LIBRARY_DEBUG)
    set_property(TARGET assimp::assimp
                 APPEND
                 PROPERTY IMPORTED_CONFIGURATIONS DEBUG)

    set_target_properties(assimp::assimp
                          PROPERTIES IMPORTED_LOCATION_DEBUG "${ASSIMP_LIBRARY_DEBUG}")
  endif()


  file(WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/assimp_version_major.cpp"
       "#include <assimp/version.h>
       int main() { return aiGetVersionMajor(); }")
  file(WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/assimp_version_minor.cpp"
       "#include <assimp/version.h>
       int main() { return aiGetVersionMinor(); }")
  file(WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/assimp_version_revision.cpp"
       "#include <assimp/version.h>
       int main() { return aiGetVersionRevision(); }")

  try_run(ASSIMP_VERSION_MAJOR ASSIMP_VERSION_MAJOR_COMPILE_RESULT
          "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
          "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/assimp_version_major.cpp"
          LINK_LIBRARIES assimp::assimp)

  try_run(ASSIMP_VERSION_MINOR ASSIMP_VERSION_MINOR_COMPILE_RESULT
          "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
          "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/assimp_version_minor.cpp"
          LINK_LIBRARIES assimp::assimp)

  try_run(ASSIMP_VERSION_REVISION ASSIMP_VERSION_REVISION_COMPILE_RESULT
          "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}"
          "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/assimp_version_revision.cpp"
          LINK_LIBRARIES assimp::assimp)


  include(FindPackageHandleStandardArgs)

  if(NOT CMAKE_CROSSCOMPILING)
    set(ASSIMP_VERSION)

    if(ASSIMP_VERSION_MAJOR_COMPILE_RESULT)
      if(ASSIMP_VERBOSE)
        message(STATUS "Findassimp: succesfully parsed major version.")
      endif()

      set(ASSIMP_VERSION "${ASSIMP_VERSION_MAJOR}")

      if(ASSIMP_VERSION_MINOR_COMPILE_RESULT)
        if(ASSIMP_VERBOSE)
          message(STATUS "Findassimp: succesfully parsed minor version.")
        endif()

        set(ASSIMP_VERSION "${ASSIMP_VERSION}.${ASSIMP_VERSION_MINOR}")

        if(ASSIMP_VERSION_REVISION_COMPILE_RESULT)
          if(ASSIMP_VERBOSE)
            message(STATUS "Findassimp: succesfully parsed revision version.")
          endif()

          set(ASSIMP_VERSION "${ASSIMP_VERSION}.${ASSIMP_VERSION_REVISION}")
        endif()
      endif()
    endif()


    if(ASSIMP_VERBOSE)
      message(STATUS "Findassimp: ASSIMP_VERSION_MAJOR: ${ASSIMP_VERSION_MAJOR}")
      message(STATUS "Findassimp: ASSIMP_VERSION_MINOR: ${ASSIMP_VERSION_MINOR}")
      message(STATUS "Findassimp: ASSIMP_VERSION_REVISION: ${ASSIMP_VERSION_REVISION}")
      message(STATUS "Findassimp: ASSIMP_VERSION: ${ASSIMP_VERSION}")
    endif()


    find_package_handle_standard_args(assimp
                                      REQUIRED_VARS ASSIMP_INCLUDE_DIRS ASSIMP_LIBRARIES
                                      VERSION_VAR ASSIMP_VERSION)

  else()
    if(ASSIMP_VERBOSE)
      message(STATUS "Findassimp: cross-compiling (CMAKE_CROSSCOMPILING is TRUE), version parsing disabled.")
    endif()

    find_package_handle_standard_args(assimp
                                      REQUIRED_VARS ASSIMP_INCLUDE_DIRS ASSIMP_LIBRARIES)
  endif()

  if(NOT assimp_FOUND)
    if(ASSIMP_VERBOSE)
      message(STATUS "Findassimp: wrong assimp library version.")
    endif()
    return()
  endif()

elseif(ASSIMP_VERBOSE)
  message(WARNING "Findassimp: target `assimp::assimp` was already created. Something went wrong in Findassimp target creation.")
endif()

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
    set_package_properties(assimp PROPERTIES DESCRIPTION "Open Asset Import Library (assimp)"
                                             URL "http://www.assimp.org/")
endif()
