# Copyright (C) 2017  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.8.2)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This file can be removed.")
endif()
# source_group(TREE) is working in CMake 3.8 and CMake 3.8.2, but not in CMake 3.8.1.
if(NOT CMAKE_VERSION VERSION_LESS 3.8.2 OR CMAKE_VERSION VERSION_EQUAL 3.8)
  return()
endif()

# This file should be included only once at the higher level of the source tree
if(COMMAND _yarp_source_group_overridden)
  return()
endif()
function(_yarp_source_group_overridden)
endfunction()

# Override source_group for CMake < 3.8 to support the TREE argument
function(source_group)
  set(options )
  set(oneValueArgs TREE PREFIX)
  set(multiValueArgs FILES)
  cmake_parse_arguments(_YSG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  if(NOT DEFINED _YSG_TREE)
    _source_group(${ARGN})
  else()
    if(DEFINED _YSG_PREFIX)
      set(_YSG_PREFIX "${_YSG_PREFIX}\\")
    endif()
    foreach(_f ${_YSG_FILES})
      get_filename_component(_f_abs "${_f}" ABSOLUTE)
      file(RELATIVE_PATH _f_rel "${_YSG_TREE}" "${_f_abs}")
      get_filename_component(_d "${_f_rel}" DIRECTORY)
      string(REPLACE "/" "\\\\" _d "${_d}")
      _source_group("${_YSG_PREFIX}${_d}" FILES "${_f}")
    endforeach()
  endif()
endfunction()
