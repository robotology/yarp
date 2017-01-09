# Copyright: (C) 2009 RobotCub Consortium
# Copyright: (C) 2016 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Paul Fitzpatrick <>
#          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# This file is the official location of the current YARP version number.

include(GitInfo)


## manually increase tweak number when required. Set to zero when
# bumping VERSION_PATCH
set(YARP_VERSION_MAJOR "2")
set(YARP_VERSION_MINOR "3")
set(YARP_VERSION_PATCH "69")
set(YARP_VERSION_TWEAK "3")

set(YARP_VERSION_ABI "1")

# Generate YARP_VERSION
if(YARP_VERSION_TWEAK)
    set(YARP_VERSION "${YARP_VERSION_MAJOR}.${YARP_VERSION_MINOR}.${YARP_VERSION_PATCH}.${YARP_VERSION_TWEAK}")
else()
    set(YARP_VERSION "${YARP_VERSION_MAJOR}.${YARP_VERSION_MINOR}.${YARP_VERSION_PATCH}")
endif()

set(YARP_GENERIC_SOVERSION "${YARP_VERSION_ABI}")

set(YARP_VERSION_SHORT ${YARP_VERSION})


# Get information from the git repository if available
git_wt_info(SOURCE_DIR "${CMAKE_SOURCE_DIR}"
            PREFIX YARP)

unset(YARP_VERSION_SOURCE)
unset(YARP_VERSION_DIRTY)
unset(YARP_VERSION_REVISION)
if(DEFINED YARP_GIT_WT_HASH)
  if(YARP_GIT_WT_TAG_REVISION GREATER 0)
    set(YARP_VERSION_REVISION ${YARP_GIT_WT_TAG_REVISION})
    string(REPLACE "-" "" _date ${YARP_GIT_WT_AUTHOR_DATE})
    set(YARP_VERSION_SOURCE "${_date}.${YARP_GIT_WT_DATE_REVISION}+git${YARP_GIT_WT_HASH_SHORT}")
  endif()
  if(YARP_GIT_WT_DIRTY)
    set(YARP_VERSION_DIRTY "dirty")
  endif()
endif()

if(DEFINED YARP_VERSION_SOURCE)
  if(NOT "${YARP_GIT_WT_TAG}" STREQUAL "v${YARP_VERSION_SHORT}")
    set(YARP_VERSION "${YARP_VERSION_SHORT}+${YARP_VERSION_SOURCE}")
  else()
    set(YARP_VERSION "${YARP_VERSION_SHORT}+${YARP_VERSION_REVISION}-${YARP_VERSION_SOURCE}")
  endif()
elseif(NOT "${YARP_GIT_WT_TAG}" STREQUAL "v${YARP_VERSION_SHORT}")
  set(YARP_VERSION "${YARP_VERSION_SHORT}~dev")
else()
  set(YARP_VERSION "${YARP_VERSION_SHORT}")
endif()
if(DEFINED YARP_VERSION_DIRTY)
  set(YARP_VERSION "${YARP_VERSION}+${YARP_VERSION_DIRTY}")
endif()

message(STATUS "YARP Version: ${YARP_VERSION_SHORT} (${YARP_VERSION})")
