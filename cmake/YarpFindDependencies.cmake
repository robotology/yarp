# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# This module checks if all the dependencies are installed and if the
# dependencies to build some parts of YARP are satisfied.
# For every dependency, it creates the following variables:
#
# YARP_USE_${Package}: Can be disabled by the user if he doesn't want to use that
#                      dependency.
# YARP_HAS_${Package}: Internal flag. It should be used to check if a part of
#                      YARP should be built. It is on if YARP_USE_${Package} is
#                      on and either the package was found or will be built.
# YARP_BUILD_${Package}: Internal flag. Used to check if YARP has to build an
#                        external package.
# YARP_BUILD_DEPS_${Package}: Internal flag. Used to check if dependencies
#                             required to build the package are available.
# YARP_HAS_SYSTEM_${Package}: Internal flag. Used to check if the package is
#                             available on the system.
# YARP_USE_SYSTEM_${Package}: This flag is shown only for packages in the
#                             extern folder that were also found on the system
#                             (TRUE by default). If this flag is enabled, the
#                             system installed library will be used instead of
#                             the version shipped with YARP.


include(YarpRenamedOption)
include(CMakeDependentOption)
include(YarpPrintFeature)

# USEFUL MACROS:

# Check if a package is installed and set some cmake variables
macro(checkandset_dependency package)

  string(TOUPPER ${package} PKG)

  # YARP_HAS_SYSTEM_${package}
  if(${package}_FOUND OR ${PKG}_FOUND)
    set(YARP_HAS_SYSTEM_${package} TRUE)
  else()
    set(YARP_HAS_SYSTEM_${package} FALSE)
  endif()

  # YARP_USE_${package}
  cmake_dependent_option(YARP_USE_${package} "Use package ${package}" TRUE
                         YARP_HAS_SYSTEM_${package} FALSE)
  if(NOT "${package}" STREQUAL "${PKG}")
    yarp_renamed_option(YARP_USE_${PKG} YARP_USE_${package}) # Deprecated since YARP 3.2
  endif()
  mark_as_advanced(YARP_USE_${package})

  # YARP_USE_SYSTEM_${package}
  set(YARP_USE_SYSTEM_${package} ${YARP_USE_${package}} CACHE INTERNAL "Use system-installed ${package}, rather than a private copy (recommended)" FORCE)
  if(NOT "${package}" STREQUAL "${PKG}")
    unset(YARP_USE_SYSTEM_${PKG} CACHE) # Deprecated since YARP 3.2
  endif()

  # YARP_HAS_${package}
  if(${YARP_HAS_SYSTEM_${package}})
    set(YARP_HAS_${package} ${YARP_USE_${package}})
  else()
    set(YARP_HAS_${package} FALSE)
  endif()

endmacro()


# Check if a package is installed or if is going to be built and set some cmake variables
macro(checkbuildandset_dependency package)

  string(TOUPPER ${package} PKG)

  # YARP_HAS_SYSTEM_${package}
  if (${package}_FOUND OR ${PKG}_FOUND)
    set(YARP_HAS_SYSTEM_${package} TRUE)
  else()
    set(YARP_HAS_SYSTEM_${package} FALSE)
  endif()

  # YARP_BUILD_DEPS_${package}
  set(YARP_BUILD_DEPS_${package} 1)
  if(${ARGC} GREATER 1)
    foreach(_dep ${ARGN})
      if(NOT YARP_HAS_${_dep})
        set(YARP_BUILD_DEPS_${package} 0)
      endif()
    endforeach()
  endif()

  # YARP_USE_${package}
  cmake_dependent_option(YARP_USE_${package} "Use package ${package}" TRUE
                         "YARP_HAS_SYSTEM_${package} OR YARP_BUILD_DEPS_${package}" FALSE)
  mark_as_advanced(YARP_USE_${package})
  if(NOT "${package}" STREQUAL "${PKG}")
    yarp_renamed_option(YARP_USE_${PKG} YARP_USE_${package}) # Deprecated since YARP 3.2
  endif()

  # YARP_USE_SYSTEM_${package}
  cmake_dependent_option(YARP_USE_SYSTEM_${package} "Use system-installed ${package}, rather than a private copy (recommended)" TRUE
                         "YARP_HAS_SYSTEM_${package};YARP_USE_${package}" FALSE)
  mark_as_advanced(YARP_USE_SYSTEM_${package})
  if(NOT "${package}" STREQUAL "${PKG}")
    yarp_renamed_option(YARP_USE_SYSTEM_${PKG} YARP_USE_SYSTEM_${package}) # Deprecated since YARP 3.2
  endif()

  # YARP_HAS_${package}
  set(YARP_HAS_${package} ${YARP_USE_${package}})

  # YARP_BUILD_${package}
  if(YARP_USE_${package} AND NOT YARP_USE_SYSTEM_${package} AND YARP_BUILD_DEPS_${package})
    set(YARP_BUILD_${package} TRUE)
  else()
    set(YARP_BUILD_${package} FALSE)
  endif()

endmacro()

# Always build some package and set some cmake variables
macro(buildandset_dependency package)

  set(YARP_HAS_SYSTEM_${package} FALSE)

  # YARP_USE_${package}
  option(YARP_USE_${package} "Use package ${package}" TRUE)
  mark_as_advanced(YARP_USE_${package})

  # YARP_HAS_${package}
  set(YARP_HAS_${package} ${YARP_USE_${package}})

  # YARP_BUILD_${package}
  set(YARP_BUILD_${package} TRUE)

  if(YARP_USE_${package})
    if(${ARGC} GREATER 1)
      foreach(_dep ${ARGN})
        string(TOUPPER ${_dep} _DEP)
        if(NOT YARP_HAS_${_DEP})
          message(WARNING "${_dep} (required to build ${package}) not found.")
          set(YARP_HAS_${package} FALSE)
          set(YARP_BUILD_${package} FALSE)
        endif()
      endforeach()
    endif()
  endif()

endmacro()




# Check if a required package is installed.
macro(check_required_dependency package)
  if(NOT YARP_HAS_${package})
    message(FATAL_ERROR "Required package ${package} not found. Please install it to build yarp.")
  endif()
endmacro()


# Check if a dependency required to enable an option is installed.
macro(check_optional_dependency optionname package)
  if(${optionname})
    if(NOT YARP_HAS_${package})
      message(FATAL_ERROR "Optional package ${package} not found. Please install it or disable the option \"${optionname}\" to build yarp.")
    endif()
  endif()
endmacro()


# Check if at least one of the dependency required to enable an option is installed.
function(check_alternative_dependency optionname)
  if(${optionname})
    foreach(package "${ARGN}")
      if(YARP_HAS_${package})
        return()
      endif()
    endforeach()
    message(FATAL_ERROR "None of the alternative packages \"${ARGN}\" was found. Please install at least one of them or disable the option \"${optionname}\" to build yarp.")
  endif()
endfunction()


# Check if a dependency required to disable an option is installed.
macro(check_skip_dependency optionname package)
  if(NOT ${optionname})
    if(NOT YARP_HAS_${package})
      message(FATAL_ERROR "Optional package ${package} not found. Please install it or enable the option \"${optionname}\" to build yarp.")
    endif()
  endif()
endmacro()


# Print status for a dependency
macro(print_dependency package)

  string(TOUPPER ${package} PKG)

#  message("YARP_HAS_SYSTEM_${package} = ${YARP_HAS_SYSTEM_${package}}")
#  message("YARP_USE_${package} = ${YARP_USE_${package}}")
#  message("YARP_USE_SYSTEM_${package} = ${YARP_USE_SYSTEM_${package}}")
#  message("YARP_HAS_${package} = ${YARP_HAS_${package}}")
#  if(NOT "${YARP_BUILD_${package}}" STREQUAL "")
#    message("YARP_BUILD_${package} = ${YARP_BUILD_${package}}")
#    message("YARP_BUILD_DEPS_${package} = ${YARP_BUILD_DEPS_${package}}")
#  endif()

  if(DEFINED ${package}_REQUIRED_VERSION)
    set(_required_version " (>= ${${package}_REQUIRED_VERSION})")
  endif()
  if(DEFINED ${package}_VERSION)
    set(_version " ${${package}_VERSION}")
  endif()
  if(NOT DEFINED YARP_HAS_${package})
    set(_reason "disabled")
  elseif(NOT YARP_HAS_${package} AND DEFINED YARP_BUILD_DEPS_${package} AND NOT YARP_BUILD_DEPS_${package})
    set(_reason "build dependencies missing")
  elseif(NOT YARP_HAS_${package})
    set(_reason "not found")
  elseif(YARP_HAS_SYSTEM_${package} AND YARP_USE_SYSTEM_${package})
    unset(_where)
    if(${package}_DIR)
      set(_where " (${${package}_DIR})")
    elseif(${package}_LIBRARIES)
      list(GET ${package}_LIBRARIES 0 _lib)
      if(_lib MATCHES "^(optimized|debug)$")
        list(GET ${package}_LIBRARIES 1 _lib)
      endif()
      set(_where " (${_lib})")
    elseif(${package}_INCLUDE_DIRS)
      list(GET ${package}_INCLUDE_DIRS 0 _incl)
      set(_where " (${_incl})")
    elseif(${package}_LIBRARY)
      set(_where " (${${package}_LIBRARY})")
    elseif(${package}_INCLUDE_DIR)
      set(_where " (${${package}_INCLUDE_DIR})")
    elseif(${PKG}_LIBRARY)
      set(_where " (${${PKG}_LIBRARY})")
    elseif(${PKG}_INCLUDE_DIR)
      set(_where " (${${PKG}_INCLUDE_DIR})")
    endif()
    set(_reason "found${_version}${_where}")
  elseif(YARP_HAS_SYSTEM_${package})
    set(_reason "compiling (system package disabled)")
  elseif(DEFINED YARP_CUSTOM_REASON_${package})
    set(_reason "compiling (${YARP_CUSTOM_REASON_${package}})")
  else()
    set(_reason "compiling (not found)")
  endif()

  yarp_print_with_checkbox(YARP_HAS_${package} "${package}${_required_version}: ${_reason}")

  unset(_lib)
  unset(_where)
  unset(_version)
  unset(_required_version)
  unset(_reason)
endmacro()


################################################################################
# Find all packages

# YCM is already searched in the main extern/ycm, therefore there is no need to
# look for it here.
checkandset_dependency(YCM)

find_package(ACE 6.0.0 QUIET)
checkandset_dependency(ACE)
# FIXME Deprecate SKIP_ACE in favour of YARP_USE_ACE
# YARP_USE_ACE is marked as internal
set_property(CACHE YARP_USE_ACE PROPERTY TYPE INTERNAL)
set_property(CACHE YARP_USE_ACE PROPERTY VALUE TRUE)

set(RobotTestingFramework_REQUIRED_VERSION 2)
find_package(RobotTestingFramework ${RobotTestingFramework_REQUIRED_VERSION} QUIET)
checkandset_dependency(RobotTestingFramework)

find_package(SQLite QUIET)
checkbuildandset_dependency(SQLite)

find_package(Libedit QUIET)
checkandset_dependency(Libedit)

set(TinyXML_REQUIRED_VERSION 2.6)
find_package(TinyXML ${TinyXML_REQUIRED_VERSION} QUIET)
checkbuildandset_dependency(TinyXML)

set(YARP_CUSTOM_REASON_xmlrpcpp "customized version")
buildandset_dependency(xmlrpcpp)

set(YARP_CUSTOM_REASON_hmac "not searched")
buildandset_dependency(hmac)

find_package(Eigen3 QUIET)
checkandset_dependency(Eigen3)

find_package(Qt5 COMPONENTS Core Widgets Gui Quick Qml Multimedia Xml PrintSupport QUIET)
checkandset_dependency(Qt5)

find_package(QCustomPlot QUIET)
checkbuildandset_dependency(QCustomPlot Qt5)

find_package(Graphviz QUIET)
checkandset_dependency(Graphviz)

set(YARP_CUSTOM_REASON_QGVCore "customized version")
find_package(QGVCore QUIET)
checkbuildandset_dependency(QGVCore Qt5 Graphviz)

set(SWIG_REQUIRED_VERSION 3.0)
find_package(SWIG ${SWIG_REQUIRED_VERSION} QUIET)
checkandset_dependency(SWIG)

# First part of workaround for OpenCV 2.4 bug https://github.com/robotology/yarp/issues/1024#issuecomment-267074067
# Can be removed once we only support OpenCV 3
get_property(OpenCV24_WORKAROUND_ORIGINAL_INCLUDE_DIRS DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
# End first part of workaround
find_package(OpenCV QUIET)
checkandset_dependency(OpenCV)
# Second part of the workaround
set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES ${OpenCV24_WORKAROUND_ORIGINAL_INCLUDE_DIRS})
# End second part of workaround

find_package(Lua QUIET)
checkandset_dependency(Lua)

set(Libdc1394_REQUIRED_VERSION 2.0)
find_package(Libdc1394 ${Libdc1394_REQUIRED_VERSION} QUIET)
checkandset_dependency(Libdc1394)

find_package(JPEG QUIET)
checkandset_dependency(JPEG)

find_package(PNG QUIET)
checkandset_dependency(PNG)

find_package(MPI QUIET)
checkandset_dependency(MPI)

set(LibOVR_REQUIRED_VERSION 1.17)
find_package(LibOVR ${LibOVR_REQUIRED_VERSION} QUIET)
checkandset_dependency(LibOVR)

find_package(GLFW3 QUIET)
checkandset_dependency(GLFW3)

find_package(GLEW QUIET)
checkandset_dependency(GLEW)

find_package(OpenGL QUIET)
checkandset_dependency(OpenGL)

find_package(FTDI QUIET)
checkandset_dependency(FTDI)

find_package(FFMPEG QUIET)
checkandset_dependency(FFMPEG)

find_package(SDL QUIET)
checkandset_dependency(SDL)

find_package(PortAudio QUIET)
checkandset_dependency(PortAudio)

set(ZFP_REQUIRED_VERSION 0.5.1)
find_package(ZFP ${ZFP_REQUIRED_VERSION} QUIET)
checkandset_dependency(ZFP)

find_package(OpenNI2 QUIET)
checkandset_dependency(OpenNI2)

find_package(realsense2 QUIET)
checkandset_dependency(realsense2)

find_package(Doxygen)
checkandset_dependency(Doxygen)

find_package(GObject QUIET)
checkandset_dependency(GObject)

find_package(GLIB2 QUIET)
checkandset_dependency(GLIB2)

set(GStreamer_REQUIRED_VERSION 1.4)
find_package(GStreamer ${GStreamer_REQUIRED_VERSION} QUIET)
checkandset_dependency(GStreamer)

set(GStreamerPluginsBase_REQUIRED_VERSION 1.4)
find_package(GStreamerPluginsBase ${GStreamerPluginsBase_REQUIRED_VERSION} COMPONENTS app QUIET)
checkandset_dependency(GStreamerPluginsBase)

set(BISON_REQUIRED_VERSION 2.5)
find_package(BISON ${BISON_REQUIRED_VERSION} QUIET)
checkandset_dependency(BISON)

find_package(FLEX QUIET)
checkandset_dependency(FLEX)

find_package(I2C QUIET)
checkandset_dependency(I2C)


find_package(Libv4l2 QUIET)
checkandset_dependency(Libv4l2)

find_package(Libv4lconvert QUIET)
checkandset_dependency(Libv4lconvert)



################################################################################
# Options

macro(YARP_DEPENDENT_OPTION _option _doc _default _deps _force)

  if(DEFINED ${_option})
    get_property(_option_strings_set CACHE ${_option} PROPERTY STRINGS SET)
    if(_option_strings_set)
      # If the user thinks he is smarter than the machine, he deserves an error
      get_property(_option_strings CACHE ${_option} PROPERTY STRINGS)
      list(GET _option_strings 0 _option_strings_first)
      string(REGEX REPLACE ".+\"(.+)\".+" "\\1" _option_strings_first "${_option_strings_first}")
      list(LENGTH _option_strings _option_strings_length)
      math(EXPR _option_strings_last_index "${_option_strings_length} - 1")
      list(GET _option_strings ${_option_strings_last_index} _option_strings_last)
      if("${${_option}}" STREQUAL "${_option_strings_last}")
        message(SEND_ERROR "That was a trick, you cannot outsmart me! I will never let you win! ${_option} stays OFF until I say so! \"${_option_strings_first}\" is needed to enable ${_option}. Now stop bothering me, and install your dependencies, if you really want to enable this option.")
      endif()
      unset(${_option} CACHE)
    endif()
  endif()

  cmake_dependent_option(${_option} "${_doc}" ${_default} "${_deps}" ${_force})

  unset(_missing_deps)
  foreach(_dep ${_deps})
    string(REGEX REPLACE " +" ";" _depx "${_dep}")
    if(NOT (${_depx}))
      list(APPEND _missing_deps "${_dep}")
    endif()
  endforeach()

  if(DEFINED _missing_deps)
    set(${_option}_disable_reason " (dependencies unsatisfied: \"${_missing_deps}\")")
    # Set a value that can be visualized on ccmake and on cmake-gui, but
    # still evaluates to false
    set(${_option} "OFF - Dependencies unsatisfied: '${_missing_deps}' - ${_option}-NOTFOUND" CACHE STRING "${_option_doc}" FORCE)
    string(REPLACE ";" "\;" _missing_deps "${_missing_deps}")
    set_property(CACHE ${_option}
                PROPERTY STRINGS "OFF - Dependencies unsatisfied: '${_missing_deps}' - ${_option}-NOTFOUND"
                                 "OFF - You can try as much as you want, but '${_missing_deps}' is needed to enable ${_option} - ${_option}-NOTFOUND"
                                 "OFF - Are you crazy or what? '${_missing_deps}' is needed to enable ${_option} - ${_option}-NOTFOUND"
                                 "OFF - Didn't I already tell you that '${_missing_deps}' is needed to enable ${_option}? - ${_option}-NOTFOUND"
                                 "OFF - Stop it! - ${_option}-NOTFOUND"
                                 "OFF - This is insane! Leave me alone! - ${_option}-NOTFOUND"
                                 "ON - All right, you win. The option is enabled. Are you happy now? You just broke the build.")
    # Set non-cache variable that will override the value in current scope
    # For parent scopes, the "-NOTFOUND ensures that the variable still
    # evaluates to false
    set(${_option} ${_force})
  endif()

endmacro()


option(SKIP_ACE "Compile YARP without ACE (Linux only, limited functionality)" OFF)
mark_as_advanced(SKIP_ACE)
if(SKIP_ACE)
  set_property(CACHE YARP_USE_ACE PROPERTY VALUE FALSE)
  unset(YARP_HAS_ACE) # Not set = disabled
endif()

yarp_dependent_option(YARP_COMPILE_libYARP_math "Create math library libYARP_math?" ON
                      YARP_HAS_Eigen3 OFF)
yarp_renamed_option(CREATE_LIB_MATH YARP_COMPILE_libYARP_math) # Deprecated since YARP 3.2

yarp_dependent_option(YARP_COMPILE_GUIS "Do you want to compile GUIs" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_HAS_Qt5" OFF)
yarp_renamed_option(CREATE_GUIS YARP_COMPILE_GUIS) # Deprecated since YARP 3.2

yarp_dependent_option(YARP_COMPILE_yarprobotinterface "Do you want to compile yarprobotinterface?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_HAS_TinyXML" OFF)
yarp_dependent_option(YARP_COMPILE_yarpmanager-console "Do you want to compile YARP Module Manager (console)?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_HAS_TinyXML" OFF)
yarp_dependent_option(YARP_COMPILE_yarpdatadumper "Do you want to compile yarpdatadumper?" ON
                      "YARP_COMPILE_EXECUTABLES" OFF)
yarp_dependent_option(YARP_COMPILE_yarpview "Do you want to compile yarpview?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5" OFF)
yarp_dependent_option(YARP_COMPILE_yarpmanager "Do you want to compile yarpmanager?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5;YARP_HAS_TinyXML" OFF)
yarp_dependent_option(YARP_COMPILE_yarplogger "Do you want to create yarplogger?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5" OFF)
yarp_dependent_option(YARP_COMPILE_yarpscope "Do you want to create yarpscope?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5;YARP_HAS_TinyXML;YARP_HAS_QCustomPlot" OFF)
yarp_dependent_option(YARP_COMPILE_yarpdataplayer "Do you want to compile yarpdataplayer?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5" OFF)
yarp_dependent_option(YARP_COMPILE_yarpmotorgui "Do you want to compile yarpmotorgui?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5" OFF)
yarp_dependent_option(YARP_COMPILE_yarpbatterygui "Do you want to compile yarpbatterygui?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5;NOT Qt5_VERSION VERSION_LESS 5.4;" OFF)
yarp_dependent_option(YARP_COMPILE_yarplaserscannergui  "Do you want to compile yarplaserscannergui?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5;YARP_HAS_OpenCV" OFF)
yarp_dependent_option(YARP_COMPILE_yarpviz "Do you want to compile yarpviz?" ON
                      "YARP_COMPILE_EXECUTABLES;YARP_COMPILE_GUIS;YARP_HAS_Qt5;YARP_HAS_Graphviz;YARP_HAS_QGVCore" OFF)

yarp_dependent_option(YARP_COMPILE_RobotTestingFramework_ADDONS "Compile Robot Testing Framework addons." ON
                      "YARP_HAS_RobotTestingFramework" OFF)

yarp_renamed_option(CREATE_YARPROBOTINTERFACE YARP_COMPILE_yarprobotinterface) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPMANAGER_CONSOLE YARP_COMPILE_yarpmanager-console) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPDATADUMPER YARP_COMPILE_yarpdatadumper) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPVIEW YARP_COMPILE_yarpview) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPMANAGER YARP_COMPILE_yarpmanager) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPLOGGER YARP_COMPILE_yarplogger) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPSCOPE YARP_COMPILE_yarpscope) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPDATAPLAYER YARP_COMPILE_yarpdataplayer) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPMOTORGUI YARP_COMPILE_yarpmotorgui) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPLASERSCANNERGUI YARP_COMPILE_yarplaserscannergui) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPBATTERYGUI YARP_COMPILE_yarpbatterygui) # Deprecated since YARP 3.2
yarp_renamed_option(CREATE_YARPVIZ YARP_COMPILE_yarpviz) # Deprecated since YARP 3.2
yarp_renamed_option(YARP_COMPILE_RTF_ADDONS YARP_COMPILE_RobotTestingFramework_ADDONS) # Deprecated since YARP 3.2


################################################################################
# Disable some parts if they are not required

set(YARP_COMPILE_libYARP_manager OFF)
if(YARP_COMPILE_yarpmanager-console OR YARP_COMPILE_yarpmanager OR YARP_COMPILE_RobotTestingFramework_ADDONS)
  set(YARP_COMPILE_libYARP_manager ON)
endif()

set(YARP_COMPILE_libYARP_profiler OFF)
if(YARP_COMPILE_yarpviz OR YARP_COMPILE_yarpmanager)
  set(YARP_COMPILE_libYARP_profiler ON)
endif()

if(NOT YARP_COMPILE_yarprobotinterface AND
   NOT YARP_COMPILE_yarpscope AND
   NOT YARP_COMPILE_libYARP_manager)
  set(YARP_BUILD_TinyXML FALSE)
  unset(YARP_HAS_TinyXML)
endif()

if(NOT YARP_COMPILE_yarpviz)
  set(YARP_BUILD_QGVCore FALSE)
  unset(YARP_HAS_QGVCore)
endif()

if(NOT YARP_COMPILE_yarpscope)
  set(YARP_BUILD_QCustomPlot FALSE)
  unset(YARP_HAS_QCustomPlot)
endif()

################################################################################
# Print dependencies status

message(STATUS "Libraries found:")

print_dependency(YCM)
print_dependency(ACE)
print_dependency(RobotTestingFramework)
print_dependency(SQLite)
print_dependency(Eigen3)
print_dependency(TinyXML)
print_dependency(xmlrpcpp)
print_dependency(hmac)
print_dependency(Qt5)
print_dependency(QCustomPlot)
print_dependency(Graphviz)
print_dependency(QGVCore)
print_dependency(Libedit)
print_dependency(SWIG)
print_dependency(OpenCV)
print_dependency(Lua)
print_dependency(LibOVR)
print_dependency(GLFW3)
print_dependency(GLEW)
print_dependency(OpenGL)
print_dependency(Libdc1394)
print_dependency(JPEG)
print_dependency(PNG)
print_dependency(MPI)
print_dependency(FTDI)
print_dependency(FFMPEG)
print_dependency(SDL)
print_dependency(PortAudio)
print_dependency(ZFP)
print_dependency(OpenNI2)
print_dependency(realsense2)
print_dependency(GLIB2)
print_dependency(GObject)
print_dependency(GStreamer)
print_dependency(GStreamerPluginsBase)
print_dependency(BISON)
print_dependency(FLEX)
print_dependency(I2C)
print_dependency(Libv4l2)
print_dependency(Libv4lconvert)

################################################################################
# Print information for user

message(STATUS "Enabled features:")

yarp_print_feature(BUILD_SHARED_LIBS 0 "Build shared libraries")
yarp_print_feature(SKIP_ACE 0 "Disable ACE library")
yarp_print_feature(YARP_NO_DEPRECATED 0 "Filter out deprecated declarations from YARP API")
yarp_print_feature(YARP_NO_DEPRECATED_WARNINGS 1 "Do not warn when using YARP deprecated declarations")

yarp_print_feature(YARP_COMPILE_libYARP_math 0 "Compile YARP_math library")

yarp_print_feature(YARP_COMPILE_EXECUTABLES 0 "Compile executables")
yarp_print_feature(YARP_COMPILE_yarprobotinterface 1 "Compile yarprobotinterface${YARP_COMPILE_yarprobotinterface_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpmanager-console 1 "Compile YARP Module Manager (console)${YARP_COMPILE_yarpmanager-console_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpdatadumper 1 "Compile yarpdatadumper${YARP_COMPILE_yarpdatadumper_disable_reason}")
yarp_print_feature(YARP_COMPILE_GUIS 1 "Compile GUIs${YARP_COMPILE_GUIS_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpview 2 "Compile yarpview${YARP_COMPILE_yarpview_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpmanager 2 "Compile yarpmanager${YARP_COMPILE_yarpmanager_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarplogger 2 "Compile yarplogger${YARP_COMPILE_yarplogger_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpscope 2 "Compile yarpscope${YARP_COMPILE_yarpscope_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpdataplayer 2 "Compile yarpdataplayer${YARP_COMPILE_yarpdataplayer_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpmotorgui 2 "Compile yarpmotorgui${YARP_COMPILE_yarpmotorgui_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarplaserscannergui 2 "Compile yarplaserscannergui${YARP_COMPILE_yarplaserscannergui_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpbatterygui 2 "Compile yarpbatterygui${YARP_COMPILE_yarpbatterygui_disable_reason}")
yarp_print_feature(YARP_COMPILE_yarpviz 2 "Compile yarpviz${YARP_COMPILE_yarpviz_disable_reason}")

yarp_print_feature(YARP_COMPILE_RobotTestingFramework_ADDONS 0 "Compile Robot Testing Framework addons")
yarp_print_feature(YARP_COMPILE_UNMAINTAINED 0 "Compile Unmaintained components")

yarp_print_feature(YARP_COMPILE_TESTS 0 "Compile and enable YARP tests")
yarp_print_feature(YARP_DISABLE_FAILING_TESTS 1 "Disable tests that fail randomly due to race conditions")
yarp_print_feature(YARP_ENABLE_BROKEN_TESTS 1 "Enable broken tests")
yarp_print_feature(YARP_ENABLE_INTEGRATION_TESTS 1 "Run integration tests")


################################################################################
# Check options consistency

check_skip_dependency(SKIP_ACE ACE)
check_required_dependency(hmac)
check_required_dependency(SQLite)
check_optional_dependency(YARP_COMPILE_BINDINGS SWIG)
check_optional_dependency(YARP_COMPILE_RobotTestingFramework_ADDONS RobotTestingFramework)
