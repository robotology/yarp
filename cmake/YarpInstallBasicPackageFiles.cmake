# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include(InstallBasicPackageFiles)

macro(YARP_INSTALL_BASIC_PACKAGE_FILES _export)
  set(_options )
  set(_oneValueArgs )
  set(_multiValueArgs DEPENDENCIES
                      PRIVATE_DEPENDENCIES)
  cmake_parse_arguments(_YIBPF "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN})

  unset(_deps)
  foreach(_dep ${_YIBPF_DEPENDENCIES})
    if("${_dep}" MATCHES "^YARP_")
      list(APPEND _deps "${_dep} HINTS \"\${CMAKE_CURRENT_LIST_DIR}/..\" NO_DEFAULT_PATH")
    else()
      # Do not add private libraries built by YARP
      string(TOUPPER "${_dep}" _DEP)
      string(REGEX REPLACE " +" "" _DEP ${_DEP})
      if(NOT ${YARP_BUILD_${_DEP}})
        list(APPEND _deps "${_dep}")
      endif()
    endif()
  endforeach()

  unset(_priv_deps)
  foreach(_dep ${_YIBPF_PRIVATE_DEPENDENCIES})
    if("${_dep}" MATCHES "^YARP_")
      list(APPEND _priv_deps "${_dep} HINTS \"\${CMAKE_CURRENT_LIST_DIR}/..\" NO_DEFAULT_PATH")
    else()
      # Do not add private libraries built by YARP
      string(TOUPPER "${_dep}" _DEP)
      string(REGEX REPLACE " +" "" _DEP ${_DEP})
      if(NOT ${YARP_BUILD_${_DEP}})
        list(APPEND _priv_deps "${_dep}")
      endif()
    endif()
  endforeach()

  install_basic_package_files(${_export}
                              VERSION ${YARP_VERSION}
                              COMPATIBILITY SameMajorVersion
                              EXPORT_DESTINATION ${_export}
                              DEPENDENCIES ${_deps}
                              PRIVATE_DEPENDENCIES ${_priv_deps}
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO
                              NO_SET_AND_CHECK_MACRO
                              NO_COMPATIBILITY_VARS
                              NAMESPACE YARP::
                              UPPERCASE_FILENAMES)
endmacro()
