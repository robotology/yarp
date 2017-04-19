# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(YarpBackupVariable)

# Let's see what we built, and record it to facilitate in-tree
# ("uninstalled") use of YARP.
get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
get_property(YARP_LIBS GLOBAL PROPERTY YARP_LIBS)
get_property(YARP_TOOLS GLOBAL PROPERTY YARP_TOOLS)

# Oops, cannot use YARP_DEFINES name, conflicts with an old variable
# that might be lurking in CMakeCache.txt as people upgrade.  Insert
# an "_ALL_" for now.
get_property(YARP_ALL_DEFINES GLOBAL PROPERTY YARP_DEFS)
get_property(YARP_HAS_MATH_LIB GLOBAL PROPERTY YARP_HAS_MATH_LIB)

message(STATUS "In-tree includes: ${YARP_INCLUDE_DIRS}")
message(STATUS "YARP libraries: ${YARP_LIBS}")

set(YARP_HAS_IDL TRUE)
set(YARP_IDL_BINARY_HINT ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
foreach(_config ${CMAKE_CONFIGURATION_TYPES})
  set(YARP_IDL_BINARY_HINT "${YARP_IDL_BINARY_HINT};${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/${_config}")
endforeach()
set(YARP_BINDINGS ${CMAKE_SOURCE_DIR}/bindings)

# Filter out from YARP_LIBRARIES all the plugins, yarpmod, yarpcar
# and all the other libraries that should not be linked by the user
set(YARP_LIBRARIES)
foreach(lib ${YARP_LIBS})
  if(NOT "${lib}" MATCHES "carrier$" AND
     NOT "${lib}" MATCHES "^yarp_" AND
     NOT "${lib}" MATCHES "^YARP_priv" AND
     NOT "${lib}" MATCHES "rtf_fixturemanager_" AND
     NOT "${lib}" STREQUAL "yarpcar" AND
     NOT "${lib}" STREQUAL "yarpmod" AND
     NOT "${lib}" STREQUAL "YARP_wire_rep_utils" AND
     NOT "${lib}" STREQUAL "YARP_manager" AND
     NOT "${lib}" STREQUAL "YARP_logger" AND
     NOT "${lib}" STREQUAL "YARP_serversql" AND
     NOT "${lib}" STREQUAL "YARP_gsl" AND
     NOT "${lib}" STREQUAL "YARP_eigen" AND
     NOT "${lib}" STREQUAL "YARP_rtf")
    list(APPEND YARP_LIBRARIES YARP::${lib})
  endif()
endforeach()

configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/template/YARPConfig.cmake.in"
                              "${CMAKE_BINARY_DIR}/YARPConfig.cmake"
                              INSTALL_DESTINATION "${CMAKE_BINARY_DIR}"
                              INSTALL_PREFIX "${CMAKE_BINARY_DIR}"
                              PATH_VARS YARP_INCLUDE_DIRS
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
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(${CMAKE_BINARY_DIR}/YARPConfigVersion.cmake
                                 VERSION ${YARP_VERSION_SHORT}
                                 COMPATIBILITY AnyNewerVersion)

# YARPTargets.cmake (build tree)
export(TARGETS ${YARP_LIBS} ${YARP_TOOLS}
       NAMESPACE YARP::
       FILE ${CMAKE_BINARY_DIR}/YARPTargets.cmake)

if(WIN32)
    set(YARP_CMAKE_DESTINATION "cmake")
    # Temporary fix to remove the outdated destination path that will
    # cause issues when looking for YARP package.
    # FIXME Remove this when this hack has been around for enough time.
    install(CODE
 "if(EXISTS \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/YARP\")
    message(STATUS \"Deleted: \\\"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/YARP\\\"\")
    file(REMOVE_RECURSE \"\$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/YARP\")
  endif()")
else()
  set(YARP_CMAKE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/YARP)
endif()

# Save variables for later
yarp_backup_variable(YARP_INCLUDE_DIRS)
yarp_backup_variable(YARP_BINDINGS)
yarp_backup_variable(YARP_MODULE_DIR)
yarp_backup_variable(YARP_IDL_BINARY_HINT)
yarp_backup_variable(YARP_INSTALL_PREFIX)

# Set up a configuration file for installed use of YARP
set(YARP_INCLUDE_DIRS "${CMAKE_INSTALL_FULL_INCLUDEDIR}")
set(YARP_BINDINGS "${CMAKE_INSTALL_FULL_DATADIR}/yarp/bindings")
set(YARP_MODULE_DIR "${CMAKE_INSTALL_FULL_DATADIR}/yarp/cmake")
set(YARP_IDL_BINARY_HINT "${CMAKE_INSTALL_FULL_BINDIR}")
set(YARP_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/template/YARPConfig.cmake.in"
                              "${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake"
                              INSTALL_DESTINATION ${YARP_CMAKE_DESTINATION}
                              PATH_VARS YARP_INCLUDE_DIRS
                                        YARP_BINDINGS
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
                              NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install(FILES ${CMAKE_BINARY_DIR}/YARPConfigForInstall.cmake RENAME YARPConfig.cmake COMPONENT configuration DESTINATION ${YARP_CMAKE_DESTINATION})
install(FILES ${CMAKE_BINARY_DIR}/YARPConfigVersion.cmake COMPONENT configuration DESTINATION ${YARP_CMAKE_DESTINATION})

# Restore variables to their original value
yarp_restore_variable(YARP_INCLUDE_DIRS)
yarp_restore_variable(YARP_MODULE_DIR)
yarp_restore_variable(YARP_IDL_BINARY_HINT)
yarp_restore_variable(YARP_BINDINGS)
yarp_restore_variable(YARP_INSTALL_PREFIX)


# YARPTargets.cmake (installed)
install(EXPORT YARP
        NAMESPACE YARP::
        DESTINATION ${YARP_CMAKE_DESTINATION}
        FILE YARPTargets.cmake)

foreach(lib ${YARP_LIBS})
  get_target_property(type ${lib} TYPE)
  if("${type}" STREQUAL "SHARED_LIBRARY")
    set_target_properties(${lib} PROPERTIES VERSION ${YARP_VERSION_SHORT}
                                            SOVERSION ${YARP_GENERIC_SOVERSION})
  endif()
endforeach(lib)
