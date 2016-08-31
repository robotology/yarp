#.rst:
# YarpPlugin
# ----------
#
# A set of macros for building YARP plugins.
#

# Copyright: (C) 2009, 2010 RobotCub Consortium
#            (C) 2013-2016 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Paul Fitzpatrick <paulfitz@alum.mit.edu>
#          Giorgio Metta <giorgio.metta@iit.it>
#          Lorenzo Natale <lorenzo.natale@iit.it>
#          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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
## To let YARP know how to initialize plugins, add lines like
## this in the CMakeLists.txt files the plugin subdirectories:
##
##   yarp_prepare_plugin(<plugin name>
##                       CATEGORY <category>
##                       TYPE <class name>
##                       INCLUDE <header>
##                       [WRAPPER <wrapper>] # used by devices only
##                       [DEFAULT <ON|OFF>]
##                       [ADVANCED <ON|OFF>]
##                       [DEPENDS <condition>]
##
## For devices and carriers, there are also specialized functions:
##
##   yarp_prepare_device(...)    # yarp_prepare_plugin(CATEGORY device ...)
##   yarp_prepare_carrier(...)   # yarp_prepare_plugin(CATEGORY carrier ...)
##
##
################################################################################


## Skip this whole file if it has already been included
if(COMMAND YARP_END_PLUGIN_LIBRARY)
  return()
endif()

include(GNUInstallDirs)
include(YarpInstallationHelpers)
include(CMakeParseArguments)
include(CMakeDependentOption)


################################################################################
#.rst:
# .. command:: yarp_begin_plugin_library
#
# Make sure that all the hooks needed for creating a plugin library are in
# place::
#
#    yarp_begin_plugin_library(<bundle_name>)
#
# Between this call, and a subsequent call to
# :command:`yarp_end_plugin_library`, any yarp plugin target created is tracked
# and added to the master plugin library.
#
# Calls to this macro may be nested.
#
macro(YARP_BEGIN_PLUGIN_LIBRARY bundle_name)
  if(X_YARP_PLUGIN_MODE)

    # If we are nested inside a larger plugin block, we don't
    # have to do anything.
    message(STATUS "nested library ${bundle_name}")
  else()
    # If we are the outermost plugin block, then we need to set up
    # everything for tracking the plugins within that block.

    # Make a record of the fact that we are now within a plugin
    set(X_YARP_PLUGIN_MODE TRUE)

    # Declare that we are starting to compile the given plugin library
    message(STATUS "starting plugin library: ${bundle_name}")

    # Choose a prefix for CMake options related to this library
    set(X_YARP_PLUGIN_PREFIX "${bundle_name}_")

    # Record the name of the plugin library name
    set(X_YARP_PLUGIN_MASTER ${bundle_name})

    # Set some properties to an empty state
    set_property(GLOBAL PROPERTY YARP_BUNDLE_PLUGINS) # list of plugins
    set_property(GLOBAL PROPERTY YARP_BUNDLE_OWNERS)  # owner library
    set_property(GLOBAL PROPERTY YARP_BUNDLE_LIBS)    # list of library targets
    set_property(GLOBAL PROPERTY YARP_BUNDLE_CODE)    # list of generated code

  endif()
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
#                       [ADVANCED <ON|OFF>]
#                       [DEPENDS <condition>]
#                       [TEMPLATE <file_name|path_to_a_file>]
#                       [TEMPLATE_DIR <dir>]
#                       [EXTRA_CONFIG <config>]
#                       [CODE <code>]        # Deprecated, used only by carriers
#                       [WRAPPER <wrapper>]  # Deprecated, used only by devices
#                       [PART <part>]        # Deprecated
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
# definition of the class.
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
# The ``DEFAULT`` option, if set, will be used to set the default value for this
# option, otherwise the plugin will be disabled by default.
# The ``DOC`` option can be used to set a description for this option.
# If the``ADVANCED`` option is enabled, this option is marked as advanced in
# CMake.
# If the ``INTERNAL`` option is enabled, this option is marked as internal in
# CMake. # FIXME TODO
#
# The plugin can be conditionally disabled, depending on some conditions (for
# example some option, only on some systems, or only if some library was found)
# by using the ``DEPENDS`` argument. For example ``DEPENDS "FOO;NOT BAR"`` will
# disable it only if at least one the conditions is false, therefore when
# ``FOO`` is false or ``BAR`` is true.
#
# FIXME Add docs for TEMPLATE and TEMPLATE_DIR
# FIXME Add docs for EXTRA_CONFIG, WRAPPER, CODE and PART
#
macro(YARP_PREPARE_PLUGIN _plugin_name)
  set(_options)
  set(_oneValueArgs TYPE
                    INCLUDE
                    CATEGORY
                    PARENT_TYPE
                    DEFAULT
                    DOC
                    ADVANCED
                    TEMPLATE
                    TEMPLATE_DIR
                    OPTION
                    CODE
                    WRAPPER)
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
    set(_YPP_DEFAULT OFF)
  endif()

  # Set up a flag to enable/disable compilation of this plugin.
  set(_plugin_fullname "${X_YARP_PLUGIN_PREFIX}${_plugin_name}")

  if(NOT DEFINED _YPP_DOC)
    set(_YPP_DOC "Enable/disable compilation of ${_plugin_fullname}")
  endif()

  if(NOT DEFINED _YPP_OPTION)
    set(_YPP_OPTION ENABLE_${_plugin_fullname})
  endif()

  if(DEFINED _YPP_DEPENDS)
    cmake_dependent_option(${_YPP_OPTION} "${_YPP_DOC}" ${_YPP_DEFAULT}
                           "${_YPP_DEPENDS}" OFF)
  else()
    option(${_YPP_OPTION} "${_YPP_DOC}" ${_YPP_DEFAULT})
  endif()
  if(_YPP_ADVANCED)
    mark_as_advanced(${_YPP_OPTION})
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
"/**
 * This file was generated by the YARP_PREPARE_PLUGIN CMake command.
 *
 * WARNING: This file will be overwritten when CMake is executed again, move it
 *          in the source directory if you want to customize it.
 */

#include <yarp/os/api.h>
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

  # If the plugin is enabled, add the appropriate source code into
  # the library source list.
  if(${_YPP_OPTION})
    # Go ahead and prepare some code to wrap this plugin.

    set(_fname ${CMAKE_CURRENT_BINARY_DIR}/yarp_plugin_${_plugin_fullname}.cpp)

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
        set(YARPPLUG_${_KEY} "_value")
        list(APPEND _extra_config_list YARPPLUG_${_KEY})
      endif()
    endforeach()
    if(DEFINED _YPP_WRAPPER)
      yarp_deprecated_warning("WRAPPER argument is deprecated. Use EXTRA_CONFIG WRAPPER=<...> instead.")
      set(YARPPLUG_WRAPPER "${_YPP_WRAPPER}")
    endif()
    if(DEFINED _YPP_CODE)
      yarp_deprecated_warning("CODE argument is deprecated. Use EXTRA_CONFIG CODE=<...> instead.")
      set(YARPPLUG_CODE "${_YPP_CODE}")
    endif()

    # Configure the file
    configure_file("${_YPP_TEMPLATE}"
                   "${_fname}"
                   @ONLY)

    # Unset all the variables used by the template
    unset(YARPPLUG_NAME)
    unset(YARPPLUG_TYPE)
    unset(YARPPLUG_INCLUDE)
    unset(YARPPLUG_CATEGORY)
    unset(YARPPLUG_PARENT_TYPE)
    unset(YARPPLUG_WRAPPER)
    unset(YARPPLUG_CODE)
    foreach(_extra_config ${_extra_config_list})
      unset(${_extra_config})
    endforeach()
    unset(_extra_config_list)

    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_PLUGINS ${_plugin_name})
    set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_CODE ${_fname})
    message(STATUS " +++ plugin ${_plugin_fullname}: enabled")
  else()
    unset(_missing_deps)
    if(DEFINED _YPP_DEPENDS)
      foreach(_dep ${_YPP_DEPENDS})
        if(NOT ${_dep})
          list(APPEND _missing_deps "${_dep}")
        endif()
      endforeach()
    endif()
    if(DEFINED _missing_deps)
      message(STATUS " --- plugin ${_plugin_fullname}: dependencies unsatisfied: \"${_missing_deps}\"")
    else()
      message(STATUS " --- plugin ${_plugin_fullname}: disabled")
    endif()
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

  # The user is adding a bone-fide plugin library.  We add it,
  # while inserting any generated source code needed for initialization.
  get_property(srcs GLOBAL PROPERTY YARP_BUNDLE_CODE)
  set_property(GLOBAL APPEND PROPERTY YARP_BUNDLE_OWNERS ${_library_name})
  add_library(${_library_name} ${_library_type} ${srcs} ${ARGN})
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
  set_property(GLOBAL PROPERTY YARP_BUNDLE_CODE)
endmacro()



################################################################################
# YARP_END_PLUGIN_LIBRARY macro finalizes a plugin library if this is
# the outermost plugin library block, otherwise it propagates
# all collected information to the plugin library block that wraps
# it.
#
macro(YARP_END_PLUGIN_LIBRARY bundle_name)
  # make sure we are the outermost plugin library, if nesting is present.
  if(NOT "${bundle_name}" STREQUAL "${X_YARP_PLUGIN_MASTER}")
    # If we are nested inside a larger plugin block, we don't
    # have to do anything.
    message(STATUS "ending nested plugin library ${bundle_name}")
  else()
    message(STATUS "ending plugin library: ${bundle_name}")
    # generate code to call all plugin initializers
    set(YARP_LIB_NAME ${X_YARP_PLUGIN_MASTER})
    get_property(devs GLOBAL PROPERTY YARP_BUNDLE_PLUGINS)
    get_property(owners GLOBAL PROPERTY YARP_BUNDLE_OWNERS)
    set(YARP_CODE_PRE)
    set(YARP_CODE_POST)
    if(NOT YARP_FORCE_DYNAMIC_PLUGINS AND NOT BUILD_SHARED_LIBS)
      foreach(dev ${devs})
        if(NOT owners)
          message(SEND_ERROR "No owner for device ${dev}, this is likely due to a previous error, check the output of CMake above.")
        endif()
        list(GET owners 0 owner)
        list(REMOVE_AT owners 0)
        set(YARP_CODE_PRE "${YARP_CODE_PRE}\nextern YARP_PLUGIN_IMPORT void add_owned_${dev}(const char *str);")
        set(YARP_CODE_POST "${YARP_CODE_POST}\n        add_owned_${dev}(\"${owner}\");")
      endforeach()
    endif()
    configure_file(${YARP_MODULE_DIR}/template/yarp_plugin_library.cpp.in
                   ${CMAKE_CURRENT_BINARY_DIR}/yarp_${X_YARP_PLUGIN_MASTER}_plugin_library.cpp @ONLY)
    get_property(code GLOBAL PROPERTY YARP_BUNDLE_CODE)
    get_property(libs GLOBAL PROPERTY YARP_BUNDLE_LIBS)
    # add the library initializer code
    add_library(${X_YARP_PLUGIN_MASTER} ${code} ${CMAKE_CURRENT_BINARY_DIR}/yarp_${X_YARP_PLUGIN_MASTER}_plugin_library.cpp)

    if(NOT YARP_FORCE_DYNAMIC_PLUGINS AND NOT BUILD_SHARED_LIBS)
      set_property(TARGET ${X_YARP_PLUGIN_MASTER} APPEND PROPERTY COMPILE_DEFINITIONS YARP_STATIC_PLUGIN)
      target_link_libraries(${X_YARP_PLUGIN_MASTER} LINK_PRIVATE ${libs})
    endif()
    # give user access to a list of all the plugin libraries
    set(${X_YARP_PLUGIN_MASTER}_LIBRARIES ${libs})
    set(X_YARP_PLUGIN_MODE FALSE) # neutralize redefined methods
  endif()
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
    set(YARP_CODE_PRE "YARP_DECLARE_DEVICES(${bundle_name})")
    set(YARP_CODE_POST "    YARP_REGISTER_DEVICES(${bundle_name})")
  endif()
  configure_file(${YARP_MODULE_DIR}/template/yarp_plugin_yarpdev_main.cpp.in
                 ${CMAKE_CURRENT_BINARY_DIR}/${bundle_name}_yarpdev.cpp @ONLY)
  add_executable(${exename} ${CMAKE_CURRENT_BINARY_DIR}/${bundle_name}_yarpdev.cpp)
  target_link_libraries(${exename} ${bundle_name})
  if(TARGET YARP_OS)
    # Building YARP
    target_link_libraries(${exename} YARP_OS YARP_init YARP_dev)
  else()
    target_link_libraries(${exename} YARP::YARP_OS YARP::YARP_init YARP::YARP_dev)
  endif()
endmacro()



################################################################################
# Deprecated macros
#
if(NOT YARP_NO_DEPRECATED)
  include(${CMAKE_CURRENT_LIST_DIR}/YarpDeprecatedWarning.cmake)

  macro(BEGIN_PLUGIN_LIBRARY)
    yarp_deprecated_warning("BEGIN_PLUGIN_LIBRARY is deprecated. Use YARP_BEGIN_PLUGIN_LIBRARY instead.")
    yarp_begin_plugin_library(${ARGN})
  endmacro()

  macro(ADD_PLUGIN_NORMALIZED)
    yarp_deprecated_warning("ADD_PLUGIN_NORMALIZED is deprecated.\nUse YARP_ADD_PLUGIN_NORMALIZED instead.")
    yarp_add_plugin_normalized(${ARGN})
  endmacro()

  macro(PREPARE_PLUGIN)
    yarp_deprecated_warning("PREPARE_PLUGIN is deprecated.\nUse YARP_PREPARE_PLUGIN instead.")
    yarp_prepare_plugin(${ARGN})
  endmacro()

  macro(PREPARE_DEVICE)
    yarp_deprecated_warning("PREPARE_DEVICE is deprecated.\nUse YARP_PREPARE_PLUGIN(CATEGORY device) instead.")
    yarp_prepare_plugin(${ARGN} CATEGORY device)
  endmacro()

  macro(YARP_PREPARE_DEVICE)
    yarp_deprecated_warning("YARP_PREPARE_DEVICE is deprecated.\nUse YARP_PREPARE_PLUGIN(CATEGORY device) instead.")
    yarp_prepare_plugin(${ARGN} CATEGORY device)
  endmacro()

  macro(PREPARE_CARRIER)
    yarp_deprecated_warning("PREPARE_CARRIER is deprecated.\nUse YARP_PREPARE_PLUGIN(CATEGORY carrier) instead.")
    yarp_prepare_plugin(${ARGN} CATEGORY carrier)
  endmacro()

  macro(YARP_PREPARE_CARRIER)
    yarp_deprecated_warning("YARP_PREPARE_CARRIER is deprecated.\nUse YARP_PREPARE_PLUGIN(CATEGORY carrier) instead.")
    yarp_prepare_plugin(${ARGN} CATEGORY carrier)
  endmacro()

  macro(END_PLUGIN_LIBRARY)
    yarp_deprecated_warning("END_PLUGIN_LIBRARY is deprecated.\nUse YARP_END_PLUGIN_LIBRARY instead.")
    yarp_end_plugin_library(${ARGN})
  endmacro()

  macro(ADD_PLUGIN_LIBRARY_EXECUTABLE)
    yarp_deprecated_warning("ADD_PLUGIN_LIBRARY_EXECUTABLE is deprecated.\nUse YARP_ADD_PLUGIN_YARPDEV_EXECUTABLE instead.")
    yarp_add_plugin_yarpdev_executable(${ARGN})
  endmacro()

  macro(YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE)
    yarp_deprecated_warning("YARP_ADD_PLUGIN_LIBRARY_EXECUTABLE is deprecated.\nUse YARP_ADD_PLUGIN_YARPDEV_EXECUTABLE instead.")
    yarp_add_plugin_yarpdev_executable(${ARGN})
  endmacro()

  macro(YARP_ADD_CARRIER_FINGERPRINT file_name)
    yarp_deprecated_warning("YARP_ADD_CARRIER_FINGERPRINT is deprecated.\nUse YARP_INSTALL instead.")
    yarp_install(FILES ${file_name}
                 COMPONENT runtime
                 DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  endmacro()

  macro(YARP_ADD_DEVICE_FINGERPRINT file_name)
    yarp_deprecated_warning("YARP_ADD_DEVICE_FINGERPRINT is deprecated.\nUse YARP_INSTALL instead.")
    yarp_install(FILES ${file_name}
                 COMPONENT runtime
                 DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  endmacro()
endif()
