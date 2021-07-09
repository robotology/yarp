# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

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

include(CMakeDependentOption)
cmake_dependent_option(YARP_DISABLE_VERSION_SOURCE OFF
                       "Disable version source when building YARP (avoid rebuilding everything every commit, please disable this option when including YARP version in bugs reports)."
                       "YARP_VERSION_SOURCE OR YARP_VERSION_DIRTY" OFF)
mark_as_advanced(YARP_DISABLE_VERSION_SOURCE)

if(DEFINED YARP_VERSION_SOURCE AND NOT YARP_DISABLE_VERSION_SOURCE)
  if(NOT "${YARP_GIT_WT_TAG}" STREQUAL "v${YARP_VERSION_SHORT}")
    set(YARP_VERSION "${YARP_VERSION_SHORT}+${YARP_VERSION_SOURCE}")
  else()
    set(YARP_VERSION "${YARP_VERSION_SHORT}+${YARP_VERSION_REVISION}-${YARP_VERSION_SOURCE}")
  endif()
elseif(NOT "${YARP_GIT_WT_TAG}" STREQUAL "v${YARP_VERSION_SHORT}")
  if(NOT YARP_DISABLE_VERSION_SOURCE)
    set(YARP_VERSION "${YARP_VERSION_SHORT}~dev")
  else()
    set(YARP_VERSION "${YARP_VERSION_SHORT}~dev-adv")
  endif()
else()
  set(YARP_VERSION "${YARP_VERSION_SHORT}")
endif()
if(DEFINED YARP_VERSION_DIRTY AND NOT YARP_DISABLE_VERSION_SOURCE)
  set(YARP_VERSION "${YARP_VERSION}+${YARP_VERSION_DIRTY}")
endif()

message(STATUS "YARP Version: ${YARP_VERSION_SHORT} (${YARP_VERSION})")
