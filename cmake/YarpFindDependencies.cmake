# Copyright (C) 2009  RobotCub Consortium
# Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
# Authors: Lorenzo Natale <lorenzo.natale@iit.it>
#          Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# This module checks if all the dependencies are installed and if the
# dependencies to build some parts of Yarp are satisfied.
# For every dependency, it creates the following variables:
#
# YARP_USE_${PACKAGE}: Can be disabled by the user if he doesn't want to use that
#                      dependency.
# YARP_HAS_${PACKAGE}: Internal flag. It should be used to check if a part of
#                      Yarp should be built. It is on if YARP_USE_${PACKAGE} is
#                      on and either the package was found or will be built.
# YARP_BUILD_${PACKAGE}: Internal flag. Used to check if yarp has to build an
#                        external package.
# YARP_USE_SYSTEM_${PACKAGE}: This flag is shown only for packages in the
#                             extern folder that were also found on the system
#                             (TRUE by default). If this flag is enabled, the
#                             system installed library will be used instead of
#                             the version shipped with Yarp.


include(YarpRenamedOption)
include(CMakeDependentOption)

# USEFUL MACROS:

# Check if a package is installed and set some cmake variables
macro(checkandset_dependency package)

  string(TOUPPER ${package} PKG)

  # YARP_HAS_SYSTEM_${PKG}
  if(${package}_FOUND OR ${PKG}_FOUND)
    set(YARP_HAS_SYSTEM_${PKG} TRUE)
  else()
    set(YARP_HAS_SYSTEM_${PKG} FALSE)
  endif()

  # YARP_USE_${PKG}
  cmake_dependent_option(YARP_USE_${PKG} "Use package ${package}" TRUE
                         YARP_HAS_SYSTEM_${PKG} FALSE)
  mark_as_advanced(YARP_USE_${PKG})

  # YARP_USE_SYSTEM_${PKG}
  set(YARP_USE_SYSTEM_${PKG} ${YARP_USE_${PKG}} CACHE INTERNAL "Use system-installed ${package}, rather than a private copy (recommended)" FORCE)

  # YARP_HAS_${PKG}
  if(${YARP_HAS_SYSTEM_${PKG}})
    set(YARP_HAS_${PKG} ${YARP_USE_${PKG}})
  else()
    set(YARP_HAS_${PKG} FALSE)
  endif()

endmacro ()


# Check if a package is installed or if is going to be built and set some cmake variables
macro(checkbuildandset_dependency package)

  string(TOUPPER ${package} PKG)

  # YARP_HAS_SYSTEM_${PKG}
  if (${package}_FOUND OR ${PKG}_FOUND)
    set(YARP_HAS_SYSTEM_${PKG} TRUE)
  else()
    set(YARP_HAS_SYSTEM_${PKG} FALSE)
  endif()

  # YARP_USE_${PKG}
  option(YARP_USE_${PKG} "Use package ${package}" TRUE)
  mark_as_advanced(YARP_USE_${PKG})

  # YARP_USE_SYSTEM_${PKG}
  cmake_dependent_option(YARP_USE_SYSTEM_${PKG} "Use system-installed ${package}, rather than a private copy (recommended)" TRUE
                         "YARP_HAS_SYSTEM_${PKG};YARP_USE_${PKG}" FALSE)
  mark_as_advanced(YARP_USE_SYSTEM_${PKG})

  # YARP_HAS_${PKG}
  set(YARP_HAS_${PKG} ${YARP_USE_${PKG}})

  # YARP_BUILD_${PKG}
  if(YARP_USE_${PKG} AND NOT YARP_USE_SYSTEM_${PKG})
    set(YARP_BUILD_${PKG} TRUE)
  else()
      set(YARP_BUILD_${PKG} FALSE)
  endif()

  if(YARP_USE_${PKG} AND NOT YARP_USE_SYSTEM_${PKG})
    if(${ARGC} GREATER 1)
      foreach(_dep ${ARGN})
        string(TOUPPER ${_dep} _DEP)
        if(NOT YARP_HAS_${_DEP})
          message(WARNING "${_dep} (required to build ${package}) not found.")
          set(YARP_HAS_${PKG} FALSE)
          set(YARP_BUILD_${PKG} FALSE)
        endif()
      endforeach()
    endif()
  endif()

endmacro()

# Always build some package and set some cmake variables
macro(buildandset_dependency package)

  string(TOUPPER ${package} PKG)

  set(YARP_HAS_SYSTEM_${PKG} FALSE)

  # YARP_USE_${PKG}
  option(YARP_USE_${PKG} "Use package ${package}" TRUE)
  mark_as_advanced(YARP_USE_${PKG})

  # YARP_HAS_${PKG}
  set(YARP_HAS_${PKG} ${YARP_USE_${PKG}})

  # YARP_BUILD_${PKG}
  set(YARP_BUILD_${PKG} TRUE)

  if(YARP_USE_${PKG})
    if(${ARGC} GREATER 1)
      foreach(_dep ${ARGN})
        string(TOUPPER ${_dep} _DEP)
        if(NOT YARP_HAS_${_DEP})
          message(WARNING "${_dep} (required to build ${package}) not found.")
          set(YARP_HAS_${PKG} FALSE)
          set(YARP_BUILD_${PKG} FALSE)
        endif()
      endforeach()
    endif()
  endif()

endmacro()




# Check if a required package is installed.
macro(check_required_dependency package)

  string(TOUPPER ${package} PKG)

  if(NOT YARP_HAS_${PKG})
      message(FATAL_ERROR "Required package ${package} not found. Please install it to build yarp.")
#  else()
#      message(STATUS "${PKG} -> OK")
  endif()

endmacro()


# Check if a dependency required to enable an option is installed.
macro(check_optional_dependency optionname package)

  string(TOUPPER ${package} PKG)

  if(${optionname})
    if(NOT YARP_HAS_${PKG})
      message(FATAL_ERROR "Optional package ${package} not found. Please install it or disable the option \"${optionname}\" to build yarp.")
#     else()
#       message(STATUS "${PKG} ${optionname} -> OK")
    endif()
#   else()
#     message(STATUS "${PKG} ${optionname} -> NOT REQUIRED")
  endif()

endmacro()


# Check if at least one of the dependency required to enable an option is installed.
function(check_alternative_dependency optionname)
  if(${optionname})
    foreach(package "${ARGN}")
      string(TOUPPER ${package} PKG)
      if(YARP_HAS_${PKG})
        return()
      endif()
    endforeach()
    message(FATAL_ERROR "None of the alternative packages \"${ARGN}\" was found. Please install at least one of them or disable the option \"${optionname}\" to build yarp.")
  endif()
endfunction()


# Check if a dependency required to disable an option is installed.
macro(check_skip_dependency optionname package)
  string(TOUPPER ${package} PKG)

  if(NOT ${optionname})
    if(NOT YARP_HAS_${PKG})
      message(FATAL_ERROR "Optional package ${package} not found. Please install it or enable the option \"${optionname}\" to build yarp.")
    endif()
  endif()
endmacro()


# Print status for a dependency
macro(print_dependency package)

  string(TOUPPER ${package} PKG)

#  message("YARP_HAS_SYSTEM_${PKG} = ${YARP_HAS_SYSTEM_${PKG}}")
#  message("YARP_USE_${PKG} = ${YARP_USE_${PKG}}")
#  message("YARP_USE_SYSTEM_${PKG} = ${YARP_USE_SYSTEM_${PKG}}")
#  message("YARP_HAS_${PKG} = ${YARP_HAS_${PKG}}")
#  if(NOT "${YARP_BUILD_${PKG}}" STREQUAL "")
#    message("YARP_BUILD_${PKG} = ${YARP_BUILD_${PKG}}")
#  endif()

  if(DEFINED ${package}_REQUIRED_VERSION)
    set(_required_version " (>= ${${package}_REQUIRED_VERSION})")
  endif()
  if(DEFINED ${package}_VERSION)
    set(_version " ${${package}_VERSION}")
  endif()
  if(NOT DEFINED YARP_HAS_${PKG})
    message(STATUS " --- ${package}${_required_version}: disabled")
  elseif(NOT YARP_HAS_${PKG})
    message(STATUS " --- ${package}${_required_version}: not found")
  elseif(YARP_HAS_SYSTEM_${PKG} AND YARP_USE_SYSTEM_${PKG})
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
    message(STATUS " +++ ${package}${_required_version}: found${_version}${_where}")
  elseif(YARP_HAS_SYSTEM_${PKG})
    message(STATUS " +++ ${package}${_required_version}: compiling (system package disabled)")
  else()
    message(STATUS " +++ ${package}${_required_version}: compiling (not found)")
  endif()
  unset(_lib)
  unset(_where)
  unset(_version)
  unset(_required_version)

endmacro()


# OPTIONS:

option(SKIP_ACE "Compile YARP without ACE (Linux only, TCP only, limited functionality)" OFF)
mark_as_advanced(SKIP_ACE)


option(CREATE_LIB_MATH "Create math library libYARP_math?" OFF)
cmake_dependent_option(CREATE_YARPROBOTINTERFACE "Do you want to compile yarprobotinterface?" ON YARP_COMPILE_EXECUTABLES OFF)
cmake_dependent_option(CREATE_YARPMANAGER_CONSOLE "Do you want to compile YARP Module Manager (console)?" ON YARP_COMPILE_EXECUTABLES OFF)
cmake_dependent_option(CREATE_YARPDATADUMPER "Do you want to compile yarpdatadumper?" ON YARP_COMPILE_EXECUTABLES OFF)
cmake_dependent_option(CREATE_GUIS "Do you want to compile GUIs" OFF YARP_COMPILE_EXECUTABLES OFF)
cmake_dependent_option(CREATE_YARPVIEW "Do you want to compile yarpview?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPMANAGER "Do you want to compile yarpmanager?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPLOGGER "Do you want to create yarplogger?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPSCOPE "Do you want to create yarpscope?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPDATAPLAYER "Do you want to compile yarpdataplayer?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPMOTORGUI "Do you want to compile yarpmotorgui?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPLASERSCANNERGUI  "Do you want to compile yarplaserscannergui?" OFF CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPBATTERYGUI "Do you want to compile yarpbatterygui?" OFF CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPVIZ "Do you want to compile yarpviz?" OFF CREATE_GUIS OFF)

yarp_renamed_option(CREATE_YMANAGER CREATE_YARPMANAGER_CONSOLE)
yarp_renamed_option(CREATE_GYARPMANAGER CREATE_YARPMANAGER)
yarp_deprecated_option(CREATE_GYARPBUILDER)
yarp_deprecated_option(CREATE_YARPBUILDER)
yarp_deprecated_option(CREATE_YARPMANAGER_PP) # Since YARP 2.3.68
yarp_deprecated_option(CREATE_LIB_MATH_USING_GSL) # Since YARP 2.3.70

if(CREATE_YARPMANAGER_CONSOLE OR CREATE_YARPMANAGER)
  set(CREATE_LIB_MANAGER ON CACHE INTERNAL "Create manager library libYARP_manager?")
else()
  unset(CREATE_LIB_MANAGER CACHE)
endif()

if(CREATE_YARPVIZ OR CREATE_YARPMANAGER)
  set(CREATE_LIB_PROFILER ON CACHE INTERNAL "Create profiler library libYARP_profiler?")
else()
  unset(CREATE_LIB_PROFILER CACHE)
endif()


message(STATUS "Detecting required libraries")
message(STATUS "CMake modules directory: ${CMAKE_MODULE_PATH}")


# FIND PACKAGES:

# YCM is located in the main CMakeLists.txt file
checkandset_dependency(YCM)

if(SKIP_ACE)
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(ACE_LIBRARIES pthread rt dl)
  endif()
else()
  find_package(ACE QUIET)
  checkandset_dependency(ACE)
  # FIXME Replace SKIP_ACE with YARP_USE_ACE
  set_property(CACHE YARP_USE_ACE PROPERTY TYPE INTERNAL)
  set_property(CACHE YARP_USE_ACE PROPERTY VALUE TRUE)
  if(SKIP_ACE)
    set_property(CACHE YARP_USE_ACE PROPERTY VALUE FALSE)
  endif()

  # __ACE_INLINE__ is needed in some configurations
  if(NOT ACE_COMPILES_WITHOUT_INLINE_RELEASE)
    foreach(_config ${YARP_OPTIMIZED_CONFIGURATIONS})
      string(TOUPPER ${_config} _CONFIG)
      set(CMAKE_C_FLAGS_${_CONFIG} "${CMAKE_C_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
      set(CMAKE_CXX_FLAGS_${_CONFIG} "${CMAKE_CXX_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
    endforeach()
  endif()

  if(NOT ACE_COMPILES_WITHOUT_INLINE_DEBUG)
    foreach(_config ${YARP_DEBUG_CONFIGURATIONS})
      string(TOUPPER ${_config} _CONFIG)
      set(CMAKE_C_FLAGS_${_CONFIG} "${CMAKE_C_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
      set(CMAKE_CXX_FLAGS_${_CONFIG} "${CMAKE_CXX_FLAGS_${_CONFIG}} -D__ACE_INLINE__")
    endforeach()
  endif()

  # On MSVC disable deprecated warnings generated by ACE
  if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WINSOCK_DEPRECATED_NO_WARNINGS")
  endif()
endif()

set(RTF_REQUIRED_VERSION 1.3.4)
find_package(RTF ${RTF_REQUIRED_VERSION} QUIET)
checkandset_dependency(RTF)

find_package(SQLite QUIET)
checkbuildandset_dependency(SQLite)

find_package(Libedit QUIET)
checkandset_dependency(Libedit)

if(CREATE_LIB_MATH)
  find_package(Eigen3 QUIET)
  checkandset_dependency(Eigen3)
endif()

if(CREATE_YARPROBOTINTERFACE OR CREATE_YARPSCOPE OR CREATE_LIB_MANAGER)
  set(TinyXML_REQUIRED_VERSION 2.6)
  find_package(TinyXML ${TinyXML_REQUIRED_VERSION} QUIET)
  checkbuildandset_dependency(TinyXML)
endif()

buildandset_dependency(xmlrpcpp)

if(CREATE_GUIS)
  find_package(Qt5 COMPONENTS Core Widgets Gui Quick Qml Multimedia Xml PrintSupport QUIET)
  checkandset_dependency(Qt5)
endif()

if(CREATE_YARPSCOPE)
  find_package(QCustomPlot QUIET)
  checkbuildandset_dependency(QCustomPlot Qt5)
endif()

if(CREATE_YARPVIZ)
  find_package(Graphviz QUIET)
  checkandset_dependency(Graphviz)
  find_package(QGVCore QUIET)
  checkbuildandset_dependency(QGVCore Qt5 Graphviz)
endif()

if(YARP_COMPILE_BINDINGS)
  set(SWIG_REQUIRED_VERSION 3.0)
  find_package(SWIG ${SWIG_REQUIRED_VERSION} QUIET)
  checkandset_dependency(SWIG)
endif()

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
yarp_deprecated_option(USE_LIBDC1394) # since YARP 2.3.68

find_package(JPEG QUIET)
checkandset_dependency(JPEG)

find_package(MPI QUIET)
checkandset_dependency(MPI)

set(LibOVR_REQUIRED_VERSION 1.3)
find_package(LibOVR ${LibOVR_REQUIRED_VERSION} QUIET)
checkandset_dependency(LibOVR)

find_package(GLFW3 QUIET)
checkandset_dependency(GLFW3)

find_package(GLEW QUIET)
checkandset_dependency(GLEW)

find_package(FTDI QUIET)
checkandset_dependency(FTDI)

# FindCUDA bug when crosscompiling
# See https://gitlab.kitware.com/cmake/cmake/issues/16509
if(NOT CMAKE_CROSSCOMPILING)
  find_package(CUDA QUIET)
else()
  set(CUDA_FOUND FALSE)
endif()
checkandset_dependency(CUDA)

find_package(FFMPEG QUIET)
checkandset_dependency(FFMPEG)

find_package(wxWidgets QUIET)
checkandset_dependency(wxWidgets)

find_package(SDL QUIET)
checkandset_dependency(SDL)

find_package(PortAudio QUIET)
checkandset_dependency(PortAudio)

find_package(NVIDIACg QUIET)
checkandset_dependency(NVIDIACg)

find_package(Libusb1 QUIET)
checkandset_dependency(Libusb1)

find_package(Stage QUIET)
checkandset_dependency(Stage)

set(ZFP_REQUIRED_VERSION 0.5.1)
find_package(ZFP ${ZFP_REQUIRED_VERSION} QUIET)
checkandset_dependency(ZFP)

find_package(OpenNI2 QUIET)
checkandset_dependency(OpenNI2)

find_package(Doxygen)
checkandset_dependency(Doxygen)

find_package(GLIB2 QUIET)
checkandset_dependency(GLIB2)

set(GStreamer_REQUIRED_VERSION 1.4)
find_package(GStreamer ${GStreamer_REQUIRED_VERSION} QUIET)
checkandset_dependency(GStreamer)

set(GStreamerPluginsBase_REQUIRED_VERSION 1.4)
find_package(GStreamerPluginsBase ${GStreamerPluginsBase_REQUIRED_VERSION} COMPONENTS app QUIET)
checkandset_dependency(GStreamerPluginsBase)

# PRINT DEPENDENCIES STATUS:

message(STATUS "I have found the following libraries:")
print_dependency(YCM)
print_dependency(ACE)
print_dependency(RTF)
print_dependency(SQLite)
print_dependency(Eigen3)
print_dependency(TinyXML)
#print_dependency(xmlrpcpp)
print_dependency(Qt5)
print_dependency(QCustomPlot)
print_dependency(Graphviz)
#print_dependency(QGVCore)
print_dependency(Libedit)
print_dependency(SWIG)
print_dependency(OpenCV)
print_dependency(Lua)
print_dependency(LibOVR)
print_dependency(GLFW3)
print_dependency(GLEW)
print_dependency(Libdc1394)
print_dependency(JPEG)
print_dependency(MPI)
print_dependency(FTDI)
print_dependency(CUDA)
print_dependency(FFMPEG)
print_dependency(wxWidgets)
print_dependency(SDL)
print_dependency(PortAudio)
print_dependency(NVIDIACg)
print_dependency(Libusb1)
print_dependency(Stage)
print_dependency(ZFP)
print_dependency(OpenNI2)
print_dependency(GLIB2)
print_dependency(GStreamer)
print_dependency(GStreamerPluginsBase)


# CHECK DEPENDENCIES:

check_skip_dependency(SKIP_ACE ACE)
check_required_dependency(SQLite)
check_optional_dependency(CREATE_LIB_MATH Eigen3)
check_optional_dependency(CREATE_LIB_MANAGER TinyXML)
check_optional_dependency(CREATE_YARPSCOPE TinyXML)
check_optional_dependency(CREATE_GUIS Qt5)
check_optional_dependency(CREATE_YARPSCOPE QCustomPlot)
check_optional_dependency(CREATE_YARPLASERSCANNERGUI OpenCV)
check_optional_dependency(CREATE_YARPVIZ Graphviz)
check_optional_dependency(YARP_COMPILE_BINDINGS SWIG)
check_optional_dependency(YARP_COMPILE_RTF_ADDONS RTF)


#########################################################################
# Print information for user (CDash)
if (CREATE_LIB_MATH)
  message(STATUS "YARP_math selected for compilation")
endif()
if (CREATE_GUIS)
  message(STATUS "GUIs selected for compilation")
endif()
