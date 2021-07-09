#.rst:
# YarpPlugin
# ----------
#
# A set of macros for building YARP plugins.
#

# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

################################################################################
##
## This file provides a set of macros for building bundles of plugins.
## Sample use:
##
##  yarp_begin_plugin_library(libname)
##    add_subdirectory(plugin1)
##    add_subdirectory(plugin2)
##    ...
##  yarp_end_plugin_library(libname)
##  yarp_add_plugin_yarpdev_executable(libnamedev libname)
##
## This sample would create two CMake targets, "libname" (a library) and
## libnamedev (an executable).
##
## It also defines a list:
##
##   ${libname_LIBRARIES}
##
## which contains a list of all plugins targets created within the plugin
## directories plugin1, plugin2, ...
##
## The executable is a test program that links and initializes the "libname"
## library, making the plugins accessible when yarp is built as static library.
##
## To let YARP know how to initialize plugins, a call to `yarp_prepare_plugin`
## should be added to the `CMakeLists.txt` file.
##
################################################################################


## Ensure that these variables are always defined after including this file
if(NOT DEFINED YARP_PLUGIN_LEVEL)
  set(YARP_PLUGIN_LEVEL "0")
endif()

if(NOT DEFINED YARP_PLUGIN_MASTER)
  set(YARP_PLUGIN_MASTER "")
endif()

## Skip this whole file if it has already been included
if(COMMAND YARP_END_PLUGIN_LIBRARY)
  return()
endif()

include(GNUInstallDirs)
include(CMakeParseArguments)
include(CMakeDependentOption)
include(${CMAKE_CURRENT_LIST_DIR}/YarpInstallationHelpers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/YarpPrintFeature.cmake)


################################################################################
#.rst:
# .. command:: yarp_begin_plugin_library
#
# Make sure that all the hooks needed for creating a plugin library are in
# place::
#
#    yarp_begin_plugin_library(<bundle_name>
#                              [QUIET]
#                              [OPTION <name> [DEFAULT <ON|OFF>]]
#                              [DOC "<plugin library documentation>"]
#                             )
#
# Between this call, and a subsequent call to
# :command:`yarp_end_plugin_library`, any yarp plugin target created is tracked
# and added to the master plugin library.
#
# Calls to this macro may be nested.
#
macro(YARP_BEGIN_PLUGIN_LIBRARY bundle_name)
  set(_options QUIET)
  set(_oneValueArgs OPTION
                    DEFAULT
                    DOC)
  set(_multiValueArgs )
  cmake_parse_arguments(_YBPL "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN} )

  if(DEFINED _YBPL_DEFAULT AND NOT DEFINED _YBPL_OPTION)
    message(FATAL_ERROR "DEFAULT argument cannot be used without OPTION")
  endif()

  yarp_colorize_string(bundle_name_col white 1 "${bundle_name}")

  if(YARP_PLUGIN_LEVEL)
    if(NOT _YBPL_DOC)
      set(_YBPL_DOC "Nested plugin library: ${bundle_name_col}")
    endif()

    if(DEFINED _YBPL_OPTION)
      message(FATAL_ERROR "Only first level libraries can have an option")
    endif()

    # If we are nested inside a larger plugin block, we don't
    # have to do anything.

  else()
    if(NOT _YBPL_DOC)
      set(_YBPL_DOC "Plugin library: ${bundle_name_col}")
    endif()

    # If we are the outermost plugin block, then we need to set up
    # everything for tracking the plugins within that block.

    # Record the name of the plugin library name
    set(YARP_PLUGIN_MASTER ${bundle_name})

    # Set some properties to an empty state
    set_property(GLOBAL PROPERTY YARP_BUNDLE_PLUGINS) # list of plugins
    set_property(GLOBAL PROPERTY YARP_BUNDLE_OWNERS)  # owner library
    set_property(GLOBAL PROPERTY YARP_BUNDLE_LIBS)    # list of library targets
    set_property(DIRECTORY PROPERTY YARP_BUNDLE_CODE) # list of generated code
    set_property(DIRECTORY PROPERTY YARP_BUNDLE_INI)  # name for the .ini file

    # Check if the plugin library should be enabled
    set(_disabled 0)
    if(DEFINED _YBPL_OPTION)
      option(${_YBPL_OPTION} "Enable/disable ${_YBPL_DOC}" ${_YBPL_DEFAULT})
      if(NOT ${_YBPL_OPTION})
        set(_disabled 1)
      endif()
      if(COMMAND add_feature_info)
        add_feature_info(${bundle_name} ${_YBPL_OPTION} "${_YBPL_DOC}.")
      endif()
    endif()
    set_property(GLOBAL PROPERTY YARP_BUNDLE_DISABLED ${_disabled})
  endif()

  if (NOT _YBPL_QUIET AND NOT YarpPlugin_QUIET)
    if(DEFINED _YBPL_OPTION)
      yarp_print_feature(${_YBPL_OPTION} ${YARP_PLUGIN_LEVEL} "${_YBPL_DOC}")
    else()
      message(STATUS "${_YBPL_DOC}")
    endif()
  endif()

  # Make a record of the fact that we are now within a plugin
  math(EXPR YARP_PLUGIN_LEVEL "${YARP_PLUGIN_LEVEL} + 1")
endmacro()


################################################################################
#.rst:
# .. command:: yarp_prepare_plugin
#
# Declare a plugin::
#
#   yarp_prepare_plugin(<plugin name>
#                       CATEGORY <category>
#                       [PARENT_TYPE <parent class name>]
#                       TYPE <class name>
#                       INCLUDE <header>
#                       [DEFAULT <ON|OFF>]
#                       [OPTION <name>]
#                       [DOC "<plugin documentation>"]
#                       [ADVANCED]
#                       [INTERNAL]
#                       [DEPENDS <condition>]
#                       [TEMPLATE <file_name|path_to_a_file>]
#                       [TEMPLATE_DIR <dir>]
#                       [EXTRA_CONFIG <config>]
#                       [QUIET]
#                      )
#
# This macro converts a plugin declaration to code, and to set up a CMake option
# for enabling or disabling the compilation of that plugin.
#
# The ``CATEGORY`` is the type of plugin implemented (for example ``carrier`` or
# ``device``). ``PARENT_TYPE``, if set, is the type corresponding to the
# ``CATEGORY`` type of plugins (for example ``yarp::os::Carrier`` or
# ``yarp::dev::Device``), and it is used in the generation of the code to
# register the plugin with YARP.
# The ``TYPE`` argument is used to specify the class name (including the
# eventual namespace) that implements the plugin.
# ``INCLUDE`` is the header file that should be included and that contains the
# definition of the class. The path to the header file should be relative to one
# of the include directories.
#
# For example, for a plugin implementing a ``carrier``, the header file
# (``foo/Bar.h``) will contain something like this::
#
#.. code-block:: c++
#
#    namespace foo {
#    class Bar : public yarp::os::Carrier
#    {
#      // Class implementation
#    };
#    }
#
# The corresponding CMake code will be something like::
#
#.. code-block:: cmake
#
#    yarp_prepare_plugin(CATEGORY carrier
#                        TYPE foo::Bar
#                        INCLUDE foo/Bar.h)
#
# If the ``OPTION`` argument is passed, it will be used to name the CMake
# option, otherwise a default name ``ENABLE_<master>_<name>[_<category>]`` will
# be used (``<category>`` is not added if the name already contains it).
# The ``DEFAULT`` argument, if set, will be used to set the default value for
# this option, otherwise the plugin will be disabled by default.
# The ``DOC`` argument can be used to set a description for this option.
# If the``ADVANCED`` option is enabled, this option is marked as advanced in
# CMake.
# If the ``INTERNAL`` option is enabled, this option is marked as internal in
# CMake, and therefore not displayed in CMake gui. This also implies
# `DEFAULT=ON` unless explicitly specified.
#
# The plugin can be conditionally disabled, depending on some conditions (for
# example some option, only on some systems, or only if some library was found)
# by using the ``DEPENDS`` argument. For example ``DEPENDS "FOO;NOT BAR"`` will
# disable it only if at least one the conditions is false, therefore when
# ``FOO`` is false or ``BAR`` is true.
#
# The `TEMPLATE` and `TEMPLATE_DIR` can be used to specify a file name and a
# directory for the template that will be configured and added to the plugin.
# If a template is not specified, a file `yarp_plugin_<CATEGORY>.cpp.in` is
# searched in current directory, in CMake module path, and in YARP cmake modules
# directory. If still it cannot be found, but the `PARENT_TYPE` argument,
# containing the name of the parent class for the plugin, was specified, then a
# default template is generated in current binary directory. Static plugins will
# not work with the default template.
#
# Each variable in the form `KEY=VALUE` passed to the `EXTRA_CONFIG` argument is
# changed to `YARPPLUG_<KEY>` and used when the template is configured. For
# example `EXTRA_CONFIG WRAPPER=foo` generates the `YARPPLUG_WRAPPER` variable
# that is then replaced in the `yarp_plugin_device.cpp.in`.
#
# If the `QUIET` argument is used, or the `YarpPlugin_QUIET` variable is set,
# the command does not print any message, except for warnings and errors.

macro(YARP_PREPARE_PLUGIN _plugin_name)
  set(_options ADVANCED
               INTERNAL
               QUIET)
  set(_oneValueArgs TYPE
                    INCLUDE
                    CATEGORY
                    PARENT_TYPE
                    OPTION
                    DOC
                    DEFAULT
                    TEMPLATE
                    TEMPLATE_DIR)
  set(_multiValueArgs DEPENDS
                      EXTRA_CONFIG)
  cmake_parse_arguments(_YPP "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN} )

  if(NOT DEFINED _YPP_TYPE OR NOT DEFINED _YPP_INCLUDE OR NOT DEFINED _YPP_CATEGORY)
    message(SEND_ERROR "Not enough information to create ${_plugin_name}
  type:     ${_YPP_TYPE}
  include:  ${_YPP_INCLUDE}
  message:  ${_YPP_CATEGORY}")
    return()
  endif()

  if(NOT DEFINED _YPP_DEFAULT)
    if(_YPP_INTERNAL)
      set(_YPP_DEFAULT ON)
    else()
      set(_YPP_DEFAULT OFF)
    endif()
  endif()

  if(NOT IS_ABSOLUTE "${_YPP_INCLUDE}")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_YPP_INCLUDE}")
      # DEPRECATED Since YARP 3.4
      message(DEPRECATION "${CMAKE_CURRENT_SOURCE_DIR}/The file \"${_YPP_INCLUDE}\" does not exist. This will not be allowed in a future release")
    else()
      get_filename_component(_abs_include "${_YPP_INCLUDE}" ABSOLUTE)
      set(_YPP_INCLUDE ${_abs_include})
    endif()
  else()
    if(NOT EXISTS "${_YPP_INCLUDE}")
      message(SEND_ERROR "The file \"${_YPP_INCLUDE}\" does not exist")
    endif()
  endif()

  # Set up a flag to enable/disable compilation of this plugin.
  if(NOT YARP_PLUGIN_MASTER STREQUAL "")
    set(_plugin_fullname "${YARP_PLUGIN_MASTER}_${_plugin_name}")
  else()
    set(_plugin_fullname "${_plugin_name}")
  endif()

  if(NOT DEFINED _YPP_DOC)
    set(_feature_doc "${_plugin_name} ${_YPP_CATEGORY}")
    set(_option_doc "Enable/disable ${_plugin_name} ${_YPP_CATEGORY}")
  else()
    set(_feature_doc "${_YPP_DOC}")
    set(_option_doc "Enable/disable ${_YPP_DOC}")
  endif()

  if(NOT DEFINED _YPP_OPTION)
    set(_YPP_OPTION ENABLE_${_plugin_fullname})
  endif()

  # Check if on previous CMake run the plugin was disabled due to missing
  # dependencies, and eventually remove the variable
  if(DEFINED ${_YPP_OPTION})
    get_property(_option_strings_set CACHE ${_YPP_OPTION} PROPERTY STRINGS SET)
    if(_option_strings_set)
      # If the user thinks he is smarter than the machine, he deserves an error
      get_property(_option_strings CACHE ${_YPP_OPTION} PROPERTY STRINGS)
      list(GET _option_strings 0 _option_strings_first)
      string(REGEX REPLACE ".+\"(.+)\".+" "\\1" _option_strings_first "${_option_strings_first}")
      list(LENGTH _option_strings _option_strings_length)
      math(EXPR _option_strings_last_index "${_option_strings_length} - 1")
      list(GET _option_strings ${_option_strings_last_index} _option_strings_last)
      if("${${_YPP_OPTION}}" STREQUAL "${_option_strings_last}")
        message(SEND_ERROR "That was a trick, you cannot outsmart me! I will never let you win! ${_YPP_OPTION} stays OFF until I say so! \"${_option_strings_first}\" is needed to build ${_plugin_name}. Now stop bothering me, and install your dependencies, if you really want this plugin.")
      endif()
      unset(${_YPP_OPTION} CACHE)
    endif()
  endif()

  # The bundle is enabled if this is a bundle and YARP_BUNDLE_DISABLED
  # is false/not set), or if this is not a bundle
  get_property(_bundle_disabled GLOBAL PROPERTY YARP_BUNDLE_DISABLED)
  set(_bundle_enabled TRUE)
  if(_bundle_disabled)
    set(_bundle_enabled FALSE)
  endif()
  cmake_dependent_option(${_YPP_OPTION} "${_option_doc}" ${_YPP_DEFAULT}
                         "${_bundle_enabled};${_YPP_DEPENDS}" OFF)
  if(${_YPP_OPTION} AND _YPP_INTERNAL)
    set_property(CACHE ${_YPP_OPTION} PROPERTY TYPE INTERNAL)
  else()
    if(_YPP_ADVANCED)
      mark_as_advanced(FORCE ${_YPP_OPTION})
    else()
      mark_as_advanced(CLEAR ${_YPP_OPTION})
    endif()
  endif()

  # Set some convenience variables based on whether the plugin
  # is enabled or disabled.
  set(ENABLE_${_plugin_fullname} ${${_YPP_OPTION}})
  set(ENABLE_${_plugin_name} ${${_YPP_OPTION}})
  if(ENABLE_${_plugin_name})
    set(SKIP_${_plugin_name} OFF)
    set(SKIP_${_plugin_fullname} OFF)
  else()
    set(SKIP_${_plugin_name} ON)
    set(SKIP_${_plugin_fullname} ON)
  endif()

  set(_disable_reason "")

  set(_ini_file_content
"###### This file is automatically generated by CMake.
[plugin ${_plugin_name}]
type ${_YPP_CATEGORY}
name ${_plugin_name}
library \@YARPPLUG_LIBRARY\@
")

  # Variables used by the templates:
  set(YARPPLUG_NAME "${_plugin_name}")
  set(YARPPLUG_TYPE "${_YPP_TYPE}")
  set(YARPPLUG_INCLUDE "${_YPP_INCLUDE}")
  set(YARPPLUG_CATEGORY "${_YPP_CATEGORY}")
  set(YARPPLUG_PARENT_TYPE "${_YPP_PARENT_TYPE}")
  unset(_extra_config_list)
  foreach(_conf ${_YPP_EXTRA_CONFIG})
    if(NOT "${_conf}" MATCHES "^(.+)=(.+)$")
      message(FATAL_ERROR "\"${_conf}\" is not in the form \"KEY=VALUE\"")
    else()
      set(_Key "${CMAKE_MATCH_1}")
      string(TOUPPER "${_Key}" _KEY)
      string(TOLOWER "${_Key}" _key)
      set(_value "${CMAKE_MATCH_2}")
      set(YARPPLUG_${_KEY} "${_value}")
      string(APPEND _ini_file_content "${_key} ${_value}\n")
      list(APPEND _extra_config_list YARPPLUG_${_KEY})
    endif()
  endforeach()

  if(NOT _bundle_enabled)
    set(_disable_reason " (bundle disabled)")

  elseif(NOT ${_YPP_OPTION})
    unset(_missing_deps)
    if(DEFINED _YPP_DEPENDS)
      foreach(_dep ${_YPP_DEPENDS})
        string(REGEX REPLACE " +" ";" _depx "${_dep}")
        if(NOT (${_depx}))
          list(APPEND _missing_deps "${_dep}")
        endif()
      endforeach()
    endif()
    if(DEFINED _missing_deps)
      set(_disable_reason " (dependencies unsatisfied: \"${_missing_deps}\")")
      # Set a value that can be visualized on ccmake and on cmake-gui, but
      # still evaluates to false (
      set(${_YPP_OPTION} "OFF - Dependencies unsatisfied: '${_missing_deps}' - ${_plugin_name}-NOTFOUND" CACHE STRING "${_option_doc}" FORCE)
      string(REPLACE ";" "\;" _missing_deps "${_missing_deps}")
      set_property(CACHE ${_YPP_OPTION}
                   PROPERTY STRINGS "OFF - Dependencies unsatisfied: '${_missing_deps}' - ${_plugin_name}-NOTFOUND"
                                    "OFF - You can try as much as you want, but '${_missing_deps}' is needed to build ${_plugin_name} - ${_plugin_name}-NOTFOUND"
                                    "OFF - Are you crazy or what? '${_missing_deps}' is needed to build ${_plugin_name} - ${_plugin_name}-NOTFOUND"
                                    "OFF - Didn't I already tell you that '${_missing_deps}' is needed to build ${_plugin_name}? - ${_plugin_name}-NOTFOUND"
                                    "OFF - Stop it! - ${_plugin_name}-NOTFOUND"
                                    "OFF - This is insane! Leave me alone! - ${_plugin_name}-NOTFOUND"
                                    "ON - All right, you win. The plugin is enabled. Are you happy now? You just broke the build.")
      # Set non-cache variable that will override the value in current scope
      # For parent scopes, the "-NOTFOUND ensures that the variable still
      # evaluates to false
      set(${_YPP_OPTION} OFF)
    endif()

  else()
    # If the plugin is enabled, add the appropriate source code into
    # the library source list.

    if(NOT DEFINED _YPP_TEMPLATE_DIR)
      set(_YPP_TEMPLATE_DIR "template")
    endif()

    # Search a template for the plugin or generate a basic one
    if(NOT DEFINED _YPP_TEMPLATE)
      set(_YPP_TEMPLATE "yarp_plugin_${_YPP_CATEGORY}.cpp.in")
    endif()
    unset(_template)
    foreach(_dir "${CMAKE_CURRENT_SOURCE_DIR}"
                 ${CMAKE_MODULE_PATH}
                 "${YARP_MODULE_DIR}")
      if(EXISTS "${_dir}/${_YPP_TEMPLATE_DIR}/${_YPP_TEMPLATE}")
        set(_template "${_dir}/${_YPP_TEMPLATE_DIR}/${_YPP_TEMPLATE}")
        break()
      elseif(EXISTS "${_dir}/${_YPP_TEMPLATE}")
        set(_template "${_dir}/${_YPP_TEMPLATE}")
        break()
      endif()
    endforeach()
    if(NOT DEFINED _template)
      if(NOT DEFINED _YPP_PARENT_TYPE)
        message(FATAL_ERROR "Template file not found and PARENT_TYPE not set. Cannot generate a plugin")
      else()
        file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${_YPP_TEMPLATE}"
"/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * This is an automatically-generated file, generated by the YARP_PREPARE_PLUGIN
 * CMake command.
 *
 * WARNING: This file will be overwritten when CMake is executed again, move it
 *          in the source directory if you want to customize it.
 */

#include <yarp/conf/api.h>
#include <yarp/os/SharedLibraryClass.h>
#include <\@YARPPLUG_INCLUDE\@>

#ifdef YARP_STATIC_PLUGIN
#  define YARP_PLUGIN_IMPORT
#  define YARP_PLUGIN_EXPORT
#else
#  define YARP_PLUGIN_IMPORT YARP_HELPER_DLL_IMPORT
#  define YARP_PLUGIN_EXPORT YARP_HELPER_DLL_EXPORT
#endif

#ifdef YARP_STATIC_PLUGIN
YARP_PLUGIN_EXPORT void add_owned_\@YARPPLUG_NAME\@(const char *owner) {
    // FIXME Register the plugin here if you want to allow static plugins
}
#endif

YARP_DEFINE_SHARED_SUBCLASS(\@YARPPLUG_NAME\@, \@YARPPLUG_TYPE\@, \@YARPPLUG_PARENT_TYPE\@)
")
        if(NOT YARP_FORCE_DYNAMIC_PLUGINS AND NOT BUILD_SHARED_LIBS)
          message(WARNING "Cannot generate static plugins. Move the file \"${CMAKE_CURRENT_BINARY_DIR}/${_YPP_TEMPLATE}\"")
        endif()

        set(_YPP_TEMPLATE "${CMAKE_CURRENT_BINARY_DIR}/${_YPP_TEMPLATE}")
      endif()
    else()
      set(_YPP_TEMPLATE "${_template}")
      unset(_template)
    endif()

    # Go ahead and prepare some code to wrap this plugin.
    set(_fname ${CMAKE_CURRENT_BINARY_DIR}/yarp_plugin_${_plugin_fullname}.cpp)

    # Configure the file
    configure_file("${_YPP_TEMPLATE}"
                   "${_fname}"
                   @ONLY)
    # Put the file in the right source group if defined
    get_property(autogen_source_group_set GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP SET)
    if(autogen_source_group_set)
      get_property(autogen_source_group GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP)
      source_group("${autogen_source_group}" FILES "${_fname}")
    endif()

    # Unset all the variables used by the template
    unset(YARPPLUG_NAME)
    unset(YARPPLUG_TYPE)
    unset(YARPPLUG_INCLUDE)
    unset(YARPPLUG_CATEGORY)
    unset(YARPPLUG_PARENT_TYPE)
    foreach(_extra_config ${_extra_config_list})
      unset(${_extra_config})
    endforeach()
    unset(_extra_config_list)

    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_PLUGINS ${_plugin_name})
    set_property(DIRECTORY APPEND PROPERTY YARP_BUNDLE_CODE ${_fname})
  endif()

  set_property(DIRECTORY APPEND PROPERTY YARP_BUNDLE_INI "${_plugin_name}.ini")
  string(MAKE_C_IDENTIFIER "${_plugin_name}.ini" _ini_id)
  set_property(DIRECTORY PROPERTY YARP_BUNDLE_INI_CONTENT_${_ini_id} ${_ini_file_content})

  if(NOT _YPP_QUIET AND NOT YarpPlugin_QUIET)
    if(${_YPP_OPTION})
      yarp_colorize_string(_plugin_fullname_col green 0 "${_plugin_fullname}")
    else()
      if("${_disable_reason}" STREQUAL "")
        yarp_colorize_string(_plugin_fullname_col yellow 0 "${_plugin_fullname}")
      else()
        yarp_colorize_string(_plugin_fullname_col red 0 "${_plugin_fullname}")
      endif()
    endif()
    yarp_print_feature("${_YPP_OPTION}" ${YARP_PLUGIN_LEVEL} "Plugin: ${_plugin_fullname_col}${_disable_reason}")
  endif()

  if(NOT _YPP_INTERNAL AND COMMAND add_feature_info)
    add_feature_info(${_plugin_fullname} ${_YPP_OPTION} "${_feature_doc}.")
  endif()

endmacro()



################################################################################
# YARP_ADD_PLUGIN macro tracks plugin libraries.  We want to
# be later able to link against them all as a group.
#
macro(YARP_ADD_PLUGIN _library_name)
  if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
    set(_library_type "MODULE")
  else()
    set(_library_type "STATIC")
  endif()

  # Each library can contain more than one plugin, if this is the case, then the
  # library must be added once per each plugin to the YARP_BUNDLE_OWNERS
  # variable in order to be able to match plugin and owner library later.
  get_property(devs GLOBAL PROPERTY YARP_BUNDLE_PLUGINS)
  get_property(owners GLOBAL PROPERTY YARP_BUNDLE_OWNERS)
  list(LENGTH devs devs_len)
  list(LENGTH owners owners_len)
  math(EXPR owned_devs "${devs_len} - ${owners_len}")
  foreach(_unused RANGE 1 ${owned_devs})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_OWNERS ${_library_name})
  endforeach()

  # The user is adding a bone-fide plugin library.  We add it,
  # while inserting any generated source code needed for initialization.
  add_library(${_library_name} ${_library_type})
  target_sources(${_library_name} PRIVATE ${ARGN})

  get_property(srcs DIRECTORY PROPERTY YARP_BUNDLE_CODE)
  target_sources(${_library_name} PRIVATE ${srcs})

  if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
    # Do not add the "lib" prefix to dynamic plugin libraries.
    # This simplifies search on different platforms and makes it easier
    # to distinguish them from linkable libraries.
    set_property(TARGET ${_library_name} PROPERTY PREFIX "")
  else()
    # Compile static plugins with -DYARP_STATIC_PLUGIN
    set_property(TARGET ${_library_name} APPEND PROPERTY COMPILE_DEFINITIONS YARP_STATIC_PLUGIN)
  endif()

  # Add the library to the list of plugin libraries.
  set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_LIBS ${_library_name})
  # Reset the list of generated source code and link libraries to empty.
  set_property(DIRECTORY PROPERTY YARP_BUNDLE_CODE)

  get_property(_ini_file_set DIRECTORY PROPERTY YARP_BUNDLE_INI SET)
  if(_ini_file_set)
    get_property(_ini_files DIRECTORY PROPERTY YARP_BUNDLE_INI)
    foreach(_ini_file IN LISTS _ini_files)
      string(MAKE_C_IDENTIFIER "${_ini_file}" _ini_id)
      get_property(_ini_file_content DIRECTORY PROPERTY YARP_BUNDLE_INI_CONTENT_${_ini_id})
      string(REPLACE "\@YARPPLUG_LIBRARY\@" "${_library_name}" _ini_file_content "${_ini_file_content}")
      file(WRITE "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${_ini_file}" "${_ini_file_content}")
      # Reset the .ini file content property
      set_property(DIRECTORY PROPERTY YARP_BUNDLE_INI_CONTENT_${_ini_id})
      set_property(TARGET ${_library_name} APPEND PROPERTY YARP_INI_FILES "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${_ini_file}")
    endforeach()
    # Reset .ini file name property
    set_property(DIRECTORY PROPERTY YARP_BUNDLE_INI)

  endif()
endmacro()



################################################################################
# YARP_END_PLUGIN_LIBRARY macro finalizes a plugin library if this is
# the outermost plugin library block, otherwise it propagates
# all collected information to the plugin library block that wraps
# it.
#
macro(YARP_END_PLUGIN_LIBRARY bundle_name)
  set(_options QUIET)
  set(_oneValueArgs )
  set(_multiValueArgs )
  cmake_parse_arguments(_YEPL "${_options}" "${_oneValueArgs}" "${_multiValueArgs}" ${ARGN} )

  # make sure we are the outermost plugin library, if nesting is present.
  if(NOT "${bundle_name}" STREQUAL "${YARP_PLUGIN_MASTER}")
    # If we are nested inside a larger plugin block, we don't
    # have to do anything.
    if (NOT _YEPL_QUIET AND NOT YarpPlugin_QUIET)
      message(STATUS "ending nested plugin library ${bundle_name}")
    endif()
  else()
    if (NOT _YEPL_QUIET AND NOT YarpPlugin_QUIET)
      message(STATUS "ending plugin library: ${bundle_name}")
    endif()
    # generate code to call all plugin initializers
    set(YARP_LIB_NAME ${YARP_PLUGIN_MASTER})
    get_property(devs GLOBAL PROPERTY YARP_BUNDLE_PLUGINS)
    get_property(owners GLOBAL PROPERTY YARP_BUNDLE_OWNERS)

    set(YARP_CODE_PRE)
    set(YARP_CODE_POST)
    if(NOT YARP_FORCE_DYNAMIC_PLUGINS AND NOT BUILD_SHARED_LIBS)
      foreach(dev ${devs})
        if("${owners}" STREQUAL "")
          message(SEND_ERROR "No owner for device ${dev}, this is likely due to a previous error, check the output of CMake above.")
          return()
        endif()
        list(GET owners 0 owner)
        list(REMOVE_AT owners 0)
        set(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern YARP_PLUGIN_IMPORT void add_owned_${dev}(const char *str);")
        set(YARP_CODE_POST "${YARP_CODE_POST}\n        add_owned_${dev}(\"${owner}\");")
      endforeach()
    endif()
    configure_file("${YARP_MODULE_DIR}/template/yarp_plugin_library.cpp.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/yarp_${YARP_PLUGIN_MASTER}_plugin_library.cpp" @ONLY)
    # Put the file in the right source group if defined
    get_property(autogen_source_group_set GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP SET)
    if(autogen_source_group_set)
      get_property(autogen_source_group GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP)
      source_group("${autogen_source_group}" FILES "${CMAKE_CURRENT_BINARY_DIR}/yarp_${YARP_PLUGIN_MASTER}_plugin_library.cpp")
    endif()

    get_property(code GLOBAL PROPERTY YARP_BUNDLE_CODE)
    get_property(libs GLOBAL PROPERTY YARP_BUNDLE_LIBS)

    # add the library initializer code
    add_library(${YARP_PLUGIN_MASTER})

    target_sources(${YARP_PLUGIN_MASTER} PRIVATE ${code} ${CMAKE_CURRENT_BINARY_DIR}/yarp_${YARP_PLUGIN_MASTER}_plugin_library.cpp)

    # this cannot be target_link_libraries, or in static builds the master
    # target will require the YARP_conf target, and this does not work well
    # with separate exports.
    # See also: https://gitlab.kitware.com/cmake/cmake/issues/18049
    target_include_directories(${YARP_PLUGIN_MASTER} PRIVATE $<TARGET_PROPERTY:YARP::YARP_conf,INTERFACE_INCLUDE_DIRECTORIES>)

    if(NOT YARP_FORCE_DYNAMIC_PLUGINS AND NOT BUILD_SHARED_LIBS)
      set_property(TARGET ${YARP_PLUGIN_MASTER} APPEND PROPERTY COMPILE_DEFINITIONS YARP_STATIC_PLUGIN)
      target_link_libraries(${YARP_PLUGIN_MASTER} PRIVATE ${libs})
    endif()
    # give user access to a list of all the plugin libraries
    set(${YARP_PLUGIN_MASTER}_LIBRARIES ${libs})
  endif()

  # Decrease YARP_PLUGIN_LEVEL
  math(EXPR YARP_PLUGIN_LEVEL "${YARP_PLUGIN_LEVEL} - 1")
endmacro()



################################################################################
# YARP_ADD_PLUGIN_YARPDEV_EXECUTABLE macro expands yarpdev executable
# for a named plugin library.
#
macro(YARP_ADD_PLUGIN_YARPDEV_EXECUTABLE exename bundle_name)
  if(YARP_FORCE_DYNAMIC_PLUGINS OR BUILD_SHARED_LIBS)
    set(YARP_CODE_PRE)
    set(YARP_CODE_POST)
  else()
    set(YARP_CODE_PRE "YARP_DECLARE_PLUGINS(${bundle_name})")
    set(YARP_CODE_POST "    YARP_REGISTER_PLUGINS(${bundle_name})")
  endif()
  configure_file("${YARP_MODULE_DIR}/template/yarp_plugin_yarpdev_main.cpp.in"
                 "${CMAKE_CURRENT_BINARY_DIR}/${bundle_name}_yarpdev.cpp" @ONLY)
  # Put the file in the right source group if defined
  get_property(autogen_source_group_set GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP SET)
  if(autogen_source_group_set)
    get_property(autogen_source_group GLOBAL PROPERTY AUTOGEN_SOURCE_GROUP)
    source_group("${autogen_source_group}" FILES "${CMAKE_CURRENT_BINARY_DIR}/${bundle_name}_yarpdev.cpp")
  endif()
  add_executable(${exename})
  target_sources(${exename} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${bundle_name}_yarpdev.cpp)
  target_link_libraries(${exename} PRIVATE ${bundle_name})
  target_link_libraries(${exename} PRIVATE YARP::YARP_os
                                           YARP::YARP_init
                                           YARP::YARP_dev)
endmacro()
