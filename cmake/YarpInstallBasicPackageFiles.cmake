# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(InstallBasicPackageFiles)

macro(YARP_INSTALL_BASIC_PACKAGE_FILES _export)
  set(_options )
  set(_oneValueArgs FIRST_TARGET
                    STATIC_CONFIG_TEMPLATE
                    COMPONENT)
  set(_multiValueArgs DEPENDENCIES
                      PRIVATE_DEPENDENCIES)
  cmake_parse_arguments(_YIBPF "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN})

  unset(_component)
  if(NOT DEFINED _YIBPF_COMPONENT)
    set(_YIBPF_COMPONENT ${_export}-dev)
  endif()

  unset(_deps)
  foreach(_dep ${_YIBPF_DEPENDENCIES})
    if("${_dep}" MATCHES "^YARP_")
      list(APPEND _deps "${_dep} HINTS \"\${CMAKE_CURRENT_LIST_DIR}/..\" NO_DEFAULT_PATH")
    else()
      # Add either the private libraries built by YARP, or the ones on the
      # system
      if(YARP_BUILD_${_dep})
        list(APPEND _deps "YARP_priv_${_dep}")
      else()
        list(APPEND _deps "${_dep}")
      endif()
    endif()
  endforeach()

  unset(_priv_deps)
  foreach(_dep ${_YIBPF_PRIVATE_DEPENDENCIES})
    if("${_dep}" MATCHES "^YARP_")
      list(APPEND _priv_deps "${_dep} HINTS \"\${CMAKE_CURRENT_LIST_DIR}/..\" NO_DEFAULT_PATH")
    else()
      # Add either the private libraries built by YARP, or the ones on the
      # system
      if(YARP_BUILD_${_dep})
        list(APPEND _deps "YARP_priv_${_dep} HINTS \"\${CMAKE_CURRENT_LIST_DIR}/..\" NO_DEFAULT_PATH")
      else()
        list(APPEND _priv_deps "${_dep}")
      endif()
    endif()
  endforeach()

  unset(_config_template)
  if(NOT BUILD_SHARED_LIBS AND DEFINED _YIBPF_STATIC_CONFIG_TEMPLATE)
    set(_config_template CONFIG_TEMPLATE "${_YIBPF_STATIC_CONFIG_TEMPLATE}")
  endif()
  install_basic_package_files(${_export}
                              ${_config_template}
                              VERSION ${YARP_VERSION}
                              COMPATIBILITY SameMajorVersion
                              EXPORT_DESTINATION "${CMAKE_BINARY_DIR}/${_export}"
                              INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${_export}"
                              DEPENDENCIES ${_deps}
                              PRIVATE_DEPENDENCIES ${_priv_deps}
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO
                              NO_SET_AND_CHECK_MACRO
                              COMPONENT ${_YIBPF_COMPONENT}
                              NAMESPACE YARP::
                              UPPERCASE_FILENAMES
                              ${_YIBPF_UNPARSED_ARGUMENTS})
endmacro()
