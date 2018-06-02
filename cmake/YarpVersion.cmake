# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# This file is the official location of the current YARP version number.

include(GitInfo)

set(YARP_SOVERSION "3")

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
