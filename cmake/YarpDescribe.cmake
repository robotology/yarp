# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(YarpBackupVariable)

set(YARP_BINDINGS "${CMAKE_SOURCE_DIR}/bindings")
set(YARP_CMAKECONFIG_DIR "${CMAKE_BINARY_DIR}")
set(YARP_YCM_MODULE_DIR "${CMAKE_SOURCE_DIR}/extern/ycm/ycm-${YCM_REQUIRED_VERSION}")

set(YARP_DEFAULT_FIND_COMPONENTS
  os
  sig
  dev
)
if(TARGET YARP::YARP_math)
  list(APPEND YARP_DEFAULT_FIND_COMPONENTS math)
endif()
if(YARP_COMPILE_EXECUTABLES)
  list(APPEND YARP_DEFAULT_FIND_COMPONENTS idl_tools)
endif()

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/template/YARPConfig.cmake.in"
  "${CMAKE_BINARY_DIR}/YARPConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_BINARY_DIR}"
  INSTALL_PREFIX "${CMAKE_BINARY_DIR}"
  PATH_VARS
    YARP_BINDINGS
    YARP_MODULE_DIR
    YARP_IDL_BINARY_HINT
    # YARP_INSTALL_PREFIX is empty for build tree
    # The following variables are used both for the relative
    # and absolute directory in YARPConfig.cmake.in using
    # @VAR@ for relative path or @PACKAGE_VAR for absolute.
    YARP_DATA_INSTALL_DIR
    YARP_CONFIG_INSTALL_DIR
    YARP_PLUGIN_MANIFESTS_INSTALL_DIR
    YARP_MODULES_INSTALL_DIR
    YARP_APPLICATIONS_INSTALL_DIR
    YARP_TEMPLATES_INSTALL_DIR
    YARP_APPLICATIONS_TEMPLATES_INSTALL_DIR
    YARP_MODULES_TEMPLATES_INSTALL_DIR
    YARP_CONTEXTS_INSTALL_DIR
    YARP_ROBOTS_INSTALL_DIR
    YARP_STATIC_PLUGINS_INSTALL_DIR
    YARP_DYNAMIC_PLUGINS_INSTALL_DIR
    YARP_QML2_IMPORT_DIR
    YARP_CMAKECONFIG_DIR
    YARP_YCM_MODULE_DIR
)

write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/YARPConfigVersion.cmake
  VERSION ${YARP_VERSION_SHORT}
  COMPATIBILITY SameMajorVersion
)

set(YARP_CMAKE_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/YARP")

# Save variables for later
yarp_backup_variable(YARP_BINDINGS)
yarp_backup_variable(YARP_MODULE_DIR)
yarp_backup_variable(YARP_IDL_BINARY_HINT)
yarp_backup_variable(YARP_INSTALL_PREFIX)

# Set up a configuration file for installed use of YARP
set(YARP_BINDINGS "${CMAKE_INSTALL_FULL_DATADIR}/yarp/bindings")
set(YARP_MODULE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/yarp/cmake")
set(YARP_IDL_BINARY_HINT "${CMAKE_INSTALL_FULL_BINDIR}")
set(YARP_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
get_filename_component(YARP_CMAKECONFIG_DIR "${CMAKE_INSTALL_PREFIX}/${YARP_CMAKE_DESTINATION}/.." REALPATH)
set(YARP_YCM_MODULE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/yarp/cmake/ycm-${YCM_REQUIRED_VERSION}")

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/template/YARPConfig.cmake.in"
  "${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake"
  INSTALL_DESTINATION ${YARP_CMAKE_DESTINATION}
  PATH_VARS YARP_BINDINGS
    YARP_MODULE_DIR
    YARP_IDL_BINARY_HINT
    YARP_INSTALL_PREFIX
    # The following variables are used both for the relative
    # and absolute directory in YARPConfig.cmake.in using
    # @VAR@ for relative path or @PACKAGE_VAR for absolute.
    YARP_DATA_INSTALL_DIR
    YARP_CONFIG_INSTALL_DIR
    YARP_PLUGIN_MANIFESTS_INSTALL_DIR
    YARP_MODULES_INSTALL_DIR
    YARP_APPLICATIONS_INSTALL_DIR
    YARP_TEMPLATES_INSTALL_DIR
    YARP_APPLICATIONS_TEMPLATES_INSTALL_DIR
    YARP_MODULES_TEMPLATES_INSTALL_DIR
    YARP_CONTEXTS_INSTALL_DIR
    YARP_ROBOTS_INSTALL_DIR
    YARP_STATIC_PLUGINS_INSTALL_DIR
    YARP_DYNAMIC_PLUGINS_INSTALL_DIR
    YARP_QML2_IMPORT_DIR
    YARP_CMAKECONFIG_DIR
    YARP_YCM_MODULE_DIR)
install(
  FILES ${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake
  RENAME YARPConfig.cmake
  COMPONENT configuration
  DESTINATION ${YARP_CMAKE_DESTINATION}
)
install(
  FILES ${CMAKE_BINARY_DIR}/YARPConfigVersion.cmake
  COMPONENT configuration
  DESTINATION ${YARP_CMAKE_DESTINATION}
)

# Restore variables to their original value
yarp_restore_variable(YARP_MODULE_DIR)
yarp_restore_variable(YARP_IDL_BINARY_HINT)
yarp_restore_variable(YARP_BINDINGS)
yarp_restore_variable(YARP_INSTALL_PREFIX)
