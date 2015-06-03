# Copyright: (C) 2013, 2015 Istituto Italiano di Tecnologia
# Authors: Elena Ceseracciu, Daniele Domenichelli
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


## Skip this whole file if it has already been included
if(COMMAND YARP_CONFIGURE_EXTERNAL_INSTALLATION)
  return()
endif()

include(CMakeParseArguments)
include(GNUInstallDirs)
include(CMakeDependentOption)


# Define CMAKE_INSTALL_QMLDIR for installing QML plugins
if(NOT DEFINED CMAKE_INSTALL_QMLDIR)
  # A library created by "add_library(MODULE)" is always considered
  # "LIBRARY" and not "RUNTIME" (ON WINDOWS DLLs are usually "RUNTIME"
  # when created by "add_library(SHARED)"). Therefore it would normally
  # end in "lib" in all the cases.
  set(CMAKE_INSTALL_QMLDIR "${CMAKE_INSTALL_LIBDIR}/qt5/qml" CACHE PATH "qml plugins (lib/qt5/qml)")
endif()
mark_as_advanced(CMAKE_INSTALL_QMLDIR)
if(NOT IS_ABSOLUTE ${CMAKE_INSTALL_QMLDIR})
  set(CMAKE_INSTALL_FULL_QMLDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_QMLDIR}")
else()
  set(CMAKE_INSTALL_FULL_QMLDIR "${CMAKE_INSTALL_QMLDIR}")
endif()



# yarp_configure_external_installation(<name>
#                                      [COMPONENT <component>]
#                                      [NO_PATH_D]
#                                      [WITH_PLUGINS])
#
# This function sets the the following CMake variables that contain
# paths relative to the installation prefix and that can be used as
# "DESTINATION" argument to the yarp_install macro to copy/install data
# into appropriate folders in the calling package's build tree and
# installation directory:
#
#  * <PACKAGE>_CONTEXTS_INSTALL_DIR for "context" folders, containing
#    configuration files and data that modules look for at runtime.
#  * <PACKAGE>_PLUGIN_MANIFESTS_INSTALL_DIR for plugin manifest files.
#  * <PACKAGE>_APPLICATIONS_INSTALL_DIR for XML files describing.
#    applications (collections of modules and connections between them)
#  * <PACKAGE>_MODULES_INSTALL_DIR for XML files describing modules
#    (including input/output ports).
#  * <PACKAGE>_ROBOTS_INSTALL_DIR for robot-specific configuration files
#  * <PACKAGE>_TEMPLATES_INSTALL_DIR generic directory for templates;
#    it is however advised to use specific applications/modules
#    templates install directories.
#  * <PACKAGE>_APPLICATIONS_TEMPLATES_INSTALL_DIR for application
#    templates (XML files with .template extension), which need to be
#    properly customized.
#  * <PACKAGE>_MODULES_TEMPLATES_INSTALL_DIR for module templates
#    (should not be needed)
#  * <PACKAGE>_DATA_INSTALL_DIR generic directory for data; it is
#    however advised to use more specific directories
#  * <PACKAGE>_CONFIG_INSTALL_DIR generic directory for configuration
#    files
#
# and the analogue variables containing absolute paths:
#
#  * <PACKAGE>_CONTEXTS_INSTALL_DIR_FULL
#  * <PACKAGE>_PLUGIN_MANIFESTS_INSTALL_DIR_FULL
#  * <PACKAGE>_APPLICATIONS_INSTALL_DIR_FULL
#  * <PACKAGE>_MODULES_INSTALL_DIR_FULL
#  * <PACKAGE>_ROBOTS_INSTALL_DIR_FULL
#  * <PACKAGE>_TEMPLATES_INSTALL_DIR_FULL
#  * <PACKAGE>_APPLICATIONS_TEMPLATES_INSTALL_DIR_FULL
#  * <PACKAGE>_MODULES_TEMPLATES_INSTALL_DIR_FULL
#  * <PACKAGE>_DATA_INSTALL_DIR_FULL
#  * <PACKAGE>_CONFIG_INSTALL_DIR_FULL
#
# Unless the NO_PATH_D option is passed, this macro checks if the
# installation directory of the package is the same as YARP's, in which
# case it sets up automatic recognition of data directories;
# otherwise, it warns the user to set up appropriately the
# YARP_DATA_DIRS environment variable.
#
# If the WITH_PLUGINS argument is passed, a plugin manifest file
# containing the search path for the plugins is generated and installed
#
# An extra COMPONENT argument can be passed to this function to set the
# component for the installed files. If not set, "configuration" will
# be used.

function(YARP_CONFIGURE_EXTERNAL_INSTALLATION _name)

  string(TOUPPER ${_name} _NAME)

  set(_options NO_PATH_D WITH_PLUGINS)
  set(_oneValueArgs INSTALL_COMPONENT)
  set(_multiValueArgs )
  cmake_parse_arguments(YCEI "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN} )

  if(NOT DEFINED YCEI_INSTALL_COMPONENT)
    set(YCEI_INSTALL_COMPONENT configuration)
  endif()


  # Generate and set variables
  set(${_NAME}_DATA_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/${_name}" CACHE INTERNAL "general data installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_CONFIG_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/config" CACHE INTERNAL "configuration files installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_PLUGIN_MANIFESTS_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/plugins" CACHE INTERNAL "plugin manifests installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_MODULES_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/modules" CACHE INTERNAL "modules' XML descriptions installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_APPLICATIONS_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/applications" CACHE INTERNAL "applications' XML descriptions installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_TEMPLATES_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/templates" CACHE INTERNAL "general templates installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_CONTEXTS_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/contexts" CACHE INTERNAL "contexts installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_APPLICATIONS_TEMPLATES_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/templates/applications" CACHE INTERNAL "application templates' installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_MODULES_TEMPLATES_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/templates/modules" CACHE INTERNAL "module templates' installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_ROBOTS_INSTALL_DIR "${${_NAME}_DATA_INSTALL_DIR}/robots" CACHE INTERNAL "robot-specific configurations installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_STATIC_PLUGINS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}" CACHE INTERNAL "static plugins installation directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_DYNAMIC_PLUGINS_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/${_name}" CACHE INTERNAL "dynamic plugins installation directory for ${_name} (relative to build/installation dir")

  foreach(_dir DATA
               CONFIG
               PLUGIN_MANIFESTS
               MODULES
               APPLICATIONS
               TEMPLATES
               CONTEXTS
               APPLICATIONS_TEMPLATES
               MODULES_TEMPLATES
               ROBOTS
               STATIC_PLUGINS
               DYNAMIC_PLUGINS)
    set(${_NAME}_${_dir}_INSTALL_DIR_FULL "${CMAKE_INSTALL_PREFIX}/${${_NAME}_${_dir}_INSTALL_DIR}")
  endforeach()

  # QML2 Import directory
  set(${_NAME}_QML2_IMPORT_DIR ${CMAKE_INSTALL_QMLDIR} CACHE INTERNAL "QML2 import directory for ${_name} (relative to build/installation dir")
  set(${_NAME}_QML2_IMPORT_DIR_FULL "${CMAKE_INSTALL_PREFIX}/${${_NAME}_QML2_IMPORT_DIR}")


  # Create and install the path.d file when required
  # If the name is yarp then no path.d is required.
  if(NOT YCEI_NO_PATH_D AND NOT "${_name}" STREQUAL "yarp")

    # If YARP is installed (YARP_INSTALL_PREFIX) and this package has
    # the same CMAKE_INSTALL_PREFIX as YARP, a path.d file is installed
    unset(_same_path)
    if(YARP_INSTALL_PREFIX)
      get_filename_component(yarp_prefix "${YARP_INSTALL_PREFIX}" ABSOLUTE)
      get_filename_component(current_prefix "${CMAKE_INSTALL_PREFIX}" ABSOLUTE)
      string(COMPARE EQUAL ${yarp_prefix}  ${current_prefix} _same_path)
      if((NOT _same_path) AND WIN32) #CMAKE appends project name to default prefix, let's also check parent directories
        get_filename_component(yarp_prefix_parent ${yarp_prefix} PATH)
        get_filename_component(current_prefix_parent ${current_prefix} PATH)
        string(COMPARE EQUAL ${yarp_prefix_parent} ${current_prefix_parent} _same_path)
      endif()
    endif()

    if(_same_path)

      # YARP installed in the same prefix. Create and install the file
      # in path.d

      set(_in_file "${CMAKE_BINARY_DIR}/CMakeFiles/path.d_${_name}.ini.in")
      set(_install_file "${CMAKE_BINARY_DIR}/CMakeFiles/path.d_${_name}_for_install.ini")
      set(_destination "${YARP_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/yarp/config/path.d")
      message(STATUS "Setting up installation of ${_name}.ini to ${_destination} folder.")

      unset(_path)
      # Create ${_name}.ini file inside build directory
      file(WRITE "${_in_file}"
"###### This file is automatically generated by CMake.
[search ${_name}]
path \"@_path@\"
")
      set(_path "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/${_name}")
      configure_file("${_in_file}" "${_install_file}" @ONLY)

      # Install the file into yarp config dir
      install(FILES "${_install_file}"
              RENAME ${_name}.ini
              COMPONENT ${YCEI_INSTALL_COMPONENT}
              DESTINATION "${_destination}")

    else()

      # YARP not installed or installed in different path. Show a
      # message to set YARP_DATA_DIRS

      if(WIN32)
        set(_path_separator ";")
      else()
        set(_path_separator ":")
      endif()

      if(NOT "$ENV{YARP_DATA_DIRS}" STREQUAL "")
        set(_data_dirs "$ENV{YARP_DATA_DIRS}")
      else()
        set(_data_dirs "${YARP_DATA_INSTALL_DIR_FULL}")
      endif()

      if(YARP_INSTALL_PREFIX)
        # Suggest to use the install tree
        set(_msg "Installation prefix is different from YARP")
        set(_path "${CMAKE_INSTALL_PREFIX}/${${_NAME}_DATA_INSTALL_DIR}")
      else()
        # If yarp is used not installed, then it is very likely that the
        # will use the build tree for this package as well.
        # Suggest to use the build tree
        set(_msg "Using YARP from build tree")
        set(_path "${CMAKE_BINARY_DIR}/${${_NAME}_DATA_INSTALL_DIR}")
      endif()
      message(STATUS "${_msg}: no file will we be installed into path.d folder, you need to set YARP_DATA_DIRS environment variable to ${_data_dirs}${_path_separator}${_path}")
    endif()
  endif()

  # Create and install the manifest file containing plugin search path
  # when requested
  if(YCEI_WITH_PLUGINS)
    cmake_dependent_option(YARP_FORCE_DYNAMIC_PLUGINS "Force YARP to create dynamically loaded plugins even if building static libraries." OFF
                           "NOT BUILD_SHARED_LIBS" OFF)
    mark_as_advanced(YARP_FORCE_DYNAMIC_PLUGINS)

    set(_in_file "${CMAKE_BINARY_DIR}/CMakeFiles/${_name}.ini.in")
    set(_build_file "${CMAKE_BINARY_DIR}/${${_NAME}_PLUGIN_MANIFESTS_INSTALL_DIR}/${_name}.ini")
    set(_install_file "${CMAKE_BINARY_DIR}/CMakeFiles/${_name}_for_install.ini")
    set(_destination "${${_NAME}_PLUGIN_MANIFESTS_INSTALL_DIR}")

    unset(_path)
    unset(_extension)
    unset(_type)
    file(WRITE "${_in_file}"
"###### This file is automatically generated by CMake.
[search ${_name}]
path \"@_path@\"
extension \"@_extension@\"
type \"@_type@\"
")

    if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
      set(_extension "${CMAKE_SHARED_MODULE_SUFFIX}")
      set(_type "shared")
    else()
      set(_extension "${CMAKE_STATIC_LIBRARY_SUFFIX}")
      set(_type "static")
    endif()

    set(_path "${CMAKE_BINARY_DIR}/${${_NAME}_DYNAMIC_PLUGINS_INSTALL_DIR}") # (build tree)
    configure_file("${_in_file}" "${_build_file}" @ONLY)

    set(_path "${CMAKE_INSTALL_PREFIX}/${${_NAME}_DYNAMIC_PLUGINS_INSTALL_DIR}") # (install tree)
    configure_file("${_in_file}" "${_install_file}" @ONLY)
    install(FILES "${_install_file}"
            RENAME ${_name}.ini
            COMPONENT "${YCEI_INSTALL_COMPONENT}"
            DESTINATION "${_destination}")

    # Temporary fix to remove the outdated path.ini file that will cause
    # issues when looking for plugins.
    # If the file is in the build tree, it is deleted immediately
    # If the file is in the install tree, it is deleted with make
    # install.
    # FIXME Remove this when this hack has been around for enough time
    #       to assert that there are no longer path.ini around.
    string(TOLOWER ${_name} _lcname)
    foreach(_f path.ini
               path_${_name}.ini
               path_${_name}mod.ini
               path_${_lcname}.ini
               path_${_lcname}mod.ini
               path_${_NAME}.ini
               path_${_NAME}mod.ini
               path_for_install.ini)
      if(EXISTS "${CMAKE_BINARY_DIR}/${_f}")
        message(STATUS "Deleted: \"${CMAKE_BINARY_DIR}/${_f}\"")
        file(REMOVE "${CMAKE_BINARY_DIR}/${_f}")
      endif()
      if(EXISTS "${CMAKE_BINARY_DIR}/${_destination}/${_f}")
        message(STATUS "Deleted: \"${CMAKE_BINARY_DIR}/${_destination}/${_f}\"")
        file(REMOVE "${CMAKE_BINARY_DIR}/${_destination}/${_f}")
      endif()
      if(EXISTS "${CMAKE_INSTALL_PREFIX}/${_f}")
        message(STATUS "The file \"${CMAKE_INSTALL_PREFIX}/${_f}\" will cause issues when looking for plugins. It will be automatically deleted when installing")
        install(CODE
 "if(EXISTS \"${CMAKE_INSTALL_PREFIX}/${_f}\")
    message(STATUS \"Deleted: \\\"${CMAKE_INSTALL_PREFIX}/${_f}\\\"\")
    file(REMOVE \"${CMAKE_INSTALL_PREFIX}/${_f}\")
  endif()")
      endif()
      if(EXISTS "${CMAKE_INSTALL_PREFIX}/${_destination}/${_f}")
        message(STATUS "The file \"${CMAKE_INSTALL_PREFIX}/${_destination}/${_f}\" will cause issues when looking for plugins. It will be automatically deleted when installing")
        install(CODE
 "if(EXISTS \"${CMAKE_INSTALL_PREFIX}/${_destination}/${_f}\")
    message(STATUS \"Deleted: \\\"${CMAKE_INSTALL_PREFIX}/${_destination}/${_f}\\\"\")
    file(REMOVE \"${CMAKE_INSTALL_PREFIX}/${_destination}/${_f}\")
  endif()")
      endif()
    endforeach()

  endif()

endfunction()



# This macro has the same signature as CMake "install" command (i.e.,
# with DESTINATION and FILES/DIRECTORY arguments); in addition to
# calling the "install" command,
# it also copies files to the build directory, keeping the same
# directory tree structure, to allow direct use of build tree without
# installation.
# In addition accepts the "PLUGINS" that builds the plugin in the
# appropriate folder, and exports it only if it is not a module
macro(YARP_INSTALL _what)

  # Ensure that we know how to handle this
  if(NOT "${_what}" MATCHES "^(FILES|DIRECTORY|PROGRAMS|TARGETS)$")
    message(FATAL_ERROR "Unknown option \"${_what}\"")
  endif()

  # Keep a copy of the arguments for later
  set(_installARGN ${ARGN})

  # Small hack to accept specific destination
  string(REGEX REPLACE ";(RUNTIME|ARCHIVE|LIBRARY|PUBLIC_HEADER|PRIVATE_HEADER|FRAMEWORK|RESOURCE);DESTINATION;"
                       ";\\1_DESTINATION;" _fixedARGN "${ARGN}")

  set(_options )
  set(_oneValueArgs DESTINATION
                    COMPONENT
                    EXPORT
                    LIBRARY_DESTINATION)
  set(_multiValueArgs PERMISSIONS
                      FILES
                      DIRECTORY
                      PROGRAMS
                      TARGETS)
  cmake_parse_arguments(_YI "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" "${_what};${_fixedARGN}")

  if("${_what}" STREQUAL "FILES" OR
     "${_what}" STREQUAL "DIRECTORY" OR
     "${_what}" STREQUAL "PROGRAMS")

    # Change DESTINATION argument 'dest' to "${CMAKE_BINARY_DIR}/${dest}"
    string(REGEX REPLACE "^${CMAKE_INSTALL_PREFIX}/" "" _YI_DESTINATION_RELATIVE ${_YI_DESTINATION})
    string(REGEX REPLACE ";DESTINATION;${_YI_DESTINATION}(;|$)" ";DESTINATION;${CMAKE_BINARY_DIR}/${_YI_DESTINATION_RELATIVE}\\1" _copyARGN "${ARGN}")

    # Remove COMPONENT argument
    string(REGEX REPLACE ";COMPONENT;${_YI_COMPONENT}" "" _copyARGN "${_copyARGN}")

    # Fix PERMISSION argument
    if ("${_what}" STREQUAL "PROGRAMS" AND NOT DEFINED _YI_PERMISSIONS)
      list(APPEND _copyARGN "FILE_PERMISSIONS;OWNER_READ;OWNER_WRITE;OWNER_EXECUTE;GROUP_READ;GROUP_EXECUTE;WORLD_READ;WORLD_EXECUTE")
    else()
      string(REGEX REPLACE ";PERMISSIONS;" ";FILE_PERMISSIONS;" _copyARGN "${_copyARGN}")
    endif()

    # Perform the copy
    file(COPY ${_copyARGN})

    # Perform the real installation
    install(${_what} ${_YI_${_what}} ${_installARGN})

  elseif("${_what}" STREQUAL "TARGETS")

    # Remove targets from arguments
    string(REGEX REPLACE "^${_YI_TARGETS};" "" _installARGN "${_installARGN}")


    unset(_targets)
    foreach(_target ${_YI_TARGETS})
      get_target_property(_type ${_target} TYPE)
      if(NOT "${_type}" STREQUAL "MODULE_LIBRARY")
        list(REMOVE_ITEM _YI_TARGETS ${_target})
        list(APPEND _targets ${_target})
      endif()
    endforeach()

    # Install normal targets without any change to the arguments
    list(LENGTH _targets _length)
    if(NOT _length EQUAL 0)
      install(TARGETS ${_targets} ${_installARGN})
    endif()

    # Install plugins
    list(LENGTH _YI_TARGETS _length)
    if(NOT _length EQUAL 0)

      # Set output directories if LIBRARY_DESTINATION is set
      if(DEFINED _YI_LIBRARY_DESTINATION)
        set_target_properties(${_YI_TARGETS} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${_YI_LIBRARY_DESTINATION}")
        foreach(_config ${CMAKE_CONFIGURATION_TYPES})
          string(TOUPPER ${_config} _CONFIG)
          set_target_properties(${_YI_TARGETS} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${_CONFIG} "${CMAKE_BINARY_DIR}/${_YI_LIBRARY_DESTINATION}/${_config}")
        endforeach()
      endif()

      # Dynamic plugins should not be exported
      string(REGEX REPLACE "EXPORT;${_YI_EXPORT};?" "" _installARGN "${_installARGN}")

      # Perform the real installation
      install(${_what} ${_YI_TARGETS} ${_installARGN})
    endif()
  endif()

endmacro()

