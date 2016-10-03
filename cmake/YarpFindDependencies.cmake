# Copyright (C) 2009  RobotCub Consortium
# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Authors: Lorenzo Natale, Daniele E. Domenichelli <daniele.domenichelli@iit.it>
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

endmacro (checkandset_dependency)


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

endmacro(checkbuildandset_dependency)


# Check if a required package is installed.
macro(check_required_dependency package)

  string(TOUPPER ${package} PKG)

  if(NOT YARP_HAS_${PKG})
      message(FATAL_ERROR "Required package ${package} not found. Please install it to build yarp.")
#  else()
#      message(STATUS "${PKG} -> OK")
  endif()

endmacro(check_required_dependency)


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

endmacro(check_optional_dependency)


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
    set(_version " (${${package}_REQUIRED_VERSION})")
  endif()
  if(NOT DEFINED YARP_HAS_${PKG})
    message(STATUS " --- ${package}${_version}: disabled")
  elseif(NOT YARP_HAS_${PKG})
    message(STATUS " --- ${package}${_version}: not found")
  elseif(YARP_HAS_SYSTEM_${PKG} AND YARP_USE_SYSTEM_${PKG})
    message(STATUS " +++ ${package}${_version}: found")
  elseif(YARP_HAS_SYSTEM_${PKG})
    message(STATUS " +++ ${package}${_version}: compiling (system package disabled)")
  else()
    message(STATUS " +++ ${package}${_version}: compiling (not found)")
  endif()
  unset(_version)

endmacro(print_dependency)


# OPTIONS:

option(SKIP_ACE "Compile YARP without ACE (Linux only, TCP only, limited functionality)" OFF)
mark_as_advanced(SKIP_ACE)


option(CREATE_LIB_MATH "Create math library libYARP_math?" OFF)
cmake_dependent_option(CREATE_LIB_MATH_USING_GSL "Create math library libYARP_math using GSL?" ON CREATE_LIB_MATH ON)
if(CREATE_LIB_MATH_USING_GSL)
  # FIXME YARP_USE_ATLAS is probably not a good choice since it can make
  #       confusion with YARP_USE_Atlas (generated by checkandset_dependency
  #       macro)
  option(YARP_USE_ATLAS "Enable to link to Atlas for BLAS" OFF)
else()
  unset(YARP_USE_ATLAS)
endif()

cmake_dependent_option(CREATE_YARPROBOTINTERFACE "Do you want to compile yarprobotinterface?" ON YARP_COMPILE_EXECUTABLES OFF)
cmake_dependent_option(CREATE_YARPMANAGER_CONSOLE "Do you want to compile YARP Module Manager (console)?" ON YARP_COMPILE_EXECUTABLES OFF)
cmake_dependent_option(CREATE_YARPDATADUMPER "Do you want to compile yarpdatadumper?" ON YARP_COMPILE_EXECUTABLES OFF)
cmake_dependent_option(CREATE_GUIS "Do you want to compile GUIs" OFF YARP_COMPILE_EXECUTABLES OFF)
cmake_dependent_option(CREATE_YARPVIEW "Do you want to compile yarpview?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPMANAGER "Do you want to compile yarpmanager?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPMANAGER_PP "Do you want to compile yarpmanager++?" OFF CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPLOGGER "Do you want to create yarplogger?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPSCOPE "Do you want to create yarpscope?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPDATAPLAYER "Do you want to compile yarpdataplayer?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPMOTORGUI "Do you want to compile yarpmotorgui?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPLASERSCANNERGUI  "Do you want to compile yarplaserscannergui?" ON CREATE_GUIS OFF)
cmake_dependent_option(CREATE_YARPBATTERYGUI "Do you want to compile yarpbatterygui?" ON CREATE_GUIS OFF)

yarp_renamed_option(CREATE_YMANAGER CREATE_YARPMANAGER_CONSOLE)
yarp_renamed_option(CREATE_GYARPMANAGER CREATE_YARPMANAGER)
yarp_deprecated_option(CREATE_GYARPBUILDER)
yarp_deprecated_option(CREATE_YARPBUILDER)

if(CREATE_YARPMANAGER_CONSOLE OR CREATE_YARPMANAGER)
  set(CREATE_LIB_MANAGER ON CACHE INTERNAL "Create manager library libYARP_manager?")
else()
  unset(CREATE_LIB_MANAGER CACHE)
endif()


message(STATUS "Detecting required libraries")
message(STATUS "CMake modules directory: ${CMAKE_MODULE_PATH}")


# FIND PACKAGES:

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
endif()

find_package(SQLite QUIET)
checkbuildandset_dependency(SQLite)

find_package(Libedit QUIET)
checkandset_dependency(Libedit)

if(CREATE_LIB_MATH)
  if(CREATE_LIB_MATH_USING_GSL)
    find_package(GSL QUIET)
    checkandset_dependency(GSL)
    if(YARP_USE_ATLAS)
      find_package(Atlas QUIET)
      checkandset_dependency(Atlas)
    endif()
  else()
    find_package(Eigen3)
    checkandset_dependency(Eigen3 QUIET)
  endif()
endif()

if(CREATE_YARPROBOTINTERFACE OR CREATE_YARPSCOPE OR CREATE_LIB_MANAGER)
  set(TinyXML_REQUIRED_VERSION 2.6)
  find_package(TinyXML ${TinyXML_REQUIRED_VERSION} QUIET)
  checkbuildandset_dependency(TinyXML)
endif()

if(CREATE_GUIS)
  find_package(Qt5 COMPONENTS Core Widgets Gui Quick Qml Multimedia Xml PrintSupport QUIET)
  checkandset_dependency(Qt5)
endif()

if(CREATE_YARPSCOPE)
  find_package(QCustomPlot QUIET)
  checkbuildandset_dependency(QCustomPlot Qt5)
endif()

if(YARP_COMPILE_BINDINGS)
  set(SWIG_REQUIRED_VERSION 1.3.29)
  find_package(SWIG ${SWIG_REQUIRED_VERSION} QUIET)
  checkandset_dependency(SWIG)
endif()

find_package(OpenCV QUIET)
checkandset_dependency(OpenCV)

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

find_package(LibOVR QUIET)
checkandset_dependency(LibOVR)

find_package(GLFW3 QUIET)
checkandset_dependency(GLFW3)

find_package(GLEW QUIET)
checkandset_dependency(GLEW)

find_package(FTDI QUIET)
checkandset_dependency(FTDI)

find_package(CUDA QUIET)
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


# PRINT DEPENDENCIES STATUS:

message(STATUS "I have found the following libraries:")
print_dependency(ACE)
print_dependency(SQLite)
print_dependency(Eigen3)
print_dependency(GSL)
print_dependency(Atlas)
print_dependency(TinyXML)
print_dependency(Qt5)
print_dependency(QCustomPlot)
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


# CHECK DEPENDENCIES:

check_skip_dependency(SKIP_ACE ACE)
check_required_dependency(SQLite)
if(CREATE_LIB_MATH_USING_GSL)
  check_optional_dependency(CREATE_LIB_MATH GSL)
else()
  check_optional_dependency(CREATE_LIB_MATH Eigen3)
endif()
check_optional_dependency(YARP_USE_ATLAS Atlas)
check_optional_dependency(CREATE_LIB_MANAGER TinyXML)
check_optional_dependency(CREATE_YARPSCOPE TinyXML)
check_optional_dependency(CREATE_GUIS Qt5)
check_optional_dependency(CREATE_YARPSCOPE QCustomPlot)
check_optional_dependency(CREATE_YARPLASERSCANNERGUI OpenCV)
check_optional_dependency(YARP_COMPILE_BINDINGS SWIG)


#########################################################################
# Print information for user (CDash)
if (CREATE_LIB_MATH)
  message(STATUS "YARP_math selected for compilation")
endif()
if (CREATE_GUIS)
  message(STATUS "GUIs selected for compilation")
endif()
