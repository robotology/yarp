# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


include(GNUInstallDirs)


macro(qtyarp_qml_plugin _target _path)
  set_property(TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS QT_PLUGIN)

  set_target_properties(${_target}
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_QMLDIR}/${_path}
      LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_QMLDIR}/${_path}
      ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_QMLDIR}/${_path}
  )
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qmldir")
    configure_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
      "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_QMLDIR}/${_path}/qmldir"
      COPYONLY
    )
  endif()
  foreach(_config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${_config} _CONFIG)
    set_target_properties(${_target}
      PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${_CONFIG} ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_QMLDIR}/${_config}/${_path}
        LIBRARY_OUTPUT_DIRECTORY_${_CONFIG} ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_QMLDIR}/${_config}/${_path}
        ARCHIVE_OUTPUT_DIRECTORY_${_CONFIG} ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_QMLDIR}/${_config}/${_path}
    )

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/qmldir")
      configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/qmldir"
        "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_QMLDIR}/${_config}/${_path}/qmldir"
        COPYONLY
      )
    endif()
  endforeach()

  # Update RPATH
  if(NOT CMAKE_SKIP_RPATH AND NOT CMAKE_SKIP_INSTALL_RPATH)
    file(RELATIVE_PATH _rel_path "${CMAKE_INSTALL_FULL_QMLDIR}/${_path}" "${CMAKE_INSTALL_FULL_LIBDIR}")
    get_target_property(_rpath "${_target}" INSTALL_RPATH)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
      list(APPEND _rpath "@loader_path/${_rel_path}")
    else()
      list(APPEND _rpath "\$ORIGIN/${_rel_path}")
    endif()
    set_target_properties("${_target}" PROPERTIES INSTALL_RPATH "${_rpath}")
  endif()
endmacro()


macro(qtyarp_use_qml_plugin)
  # Remove -rdynamic from CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS.
  # This is needed for some weird reason, or executables will not load resources
  # from the plugins.
  string(REPLACE "-rdynamic" "" CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS}")

  # Configure config.h file
  set(_config_h "${CMAKE_CURRENT_BINARY_DIR}/config.h")
  file(WRITE "${_config_h}.in"
"#ifndef QTYARP_CONFIG_H
#define QTYARP_CONFIG_H

#define PLUGINS_RELATIVE_PATH \"@PLUGINS_RELATIVE_PATH@\"

#endif // QTYARP_CONFIG_H
")
  set(PLUGINS_RELATIVE_PATH "../${CMAKE_INSTALL_QMLDIR}")
  configure_file("${_config_h}.in" "${_config_h}" @ONLY)

  # Include current binary dir to be able to find the config.h file
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
endmacro()


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
