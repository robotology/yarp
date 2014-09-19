# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


include(GNUInstallDirs)


macro(qtyarp_deprecate_with_cmake_version _version)
  if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS ${_version})
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION = ${CMAKE_MINIMUM_REQUIRED_VERSION}. You can remove this.")
  endif()
endmacro()


macro(qtyarp_qml_plugin _target _path)
  set_property(TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS QT_PLUGIN)

  set_target_properties(${_target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${_path}
                                              ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${_path})
  foreach(config ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${config} CONFIG)
    set_target_properties(${_target} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${_path}
                                                ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${_path})
  endforeach()

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/qmldir)
    add_custom_command(TARGET ${_target}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different
                               ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
                               ${CMAKE_BINARY_DIR}/${CMAKE_CFG_INTDIR}/${_path}/qmldir)
  endif()
endmacro()


# Hide qt5_use_modules function (that generates several warnings), when
# it can be replaced by target_link_libraries (CMake 2.8.11 or later)
# NOTE: when CMake minimum required version is 2.8.11 or later,
#       these calls should be replaced with target_link_libraries.
qtyarp_deprecate_with_cmake_version(2.8.11)
if(NOT ${CMAKE_VERSION} VERSION_LESS 2.8.11)
  macro(qt5_use_modules _target)
    foreach(_qt5lib ${ARGN})
      target_link_libraries(${_target} Qt5::${_qt5lib})
    endforeach()
  endmacro()
endif()


# Instruct CMake to issue deprecation warnings for macros and functions.
set(CMAKE_WARN_DEPRECATED TRUE)


# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)


# Configure config.h file
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/config.h.in"
"#ifndef QTYARP_CONFIG_H
#define QTYARP_CONFIG_H

#define PLUGINS_RELATIVE_PATH \"@PLUGINS_RELATIVE_PATH@\"

#endif // QTYARP_CONFIG_H
")
file(TO_NATIVE_PATH "../${CMAKE_INSTALL_QMLDIR}" PLUGINS_RELATIVE_PATH)
if(WIN32)
  string(REPLACE "\\" "\\\\" PLUGINS_RELATIVE_PATH "${PLUGINS_RELATIVE_PATH}")
endif()
configure_file("${CMAKE_CURRENT_BINARY_DIR}/config.h.in" "${CMAKE_CURRENT_BINARY_DIR}/config.h" ONLY)


# Include current binary dir to be able to find the config.h file
include_directories(${CMAKE_CURRENT_BINARY_DIR})
