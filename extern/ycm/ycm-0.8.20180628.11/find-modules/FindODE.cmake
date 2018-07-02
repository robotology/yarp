#.rst:
# FindODE
# -------
#
# Options::
#
#  ODE_DOUBLE_PRECISION  -use double precision ode libraries
#  ODE_STATIC            -link against static libraries
#
# On exit create the following variables::
#
#  ODE_INCLUDE_DIRS  - Directories to include to use ODE
#  ODE_LIBRARIES     - Default library to link against to use ODE
#  ODE_FOUND         - If false, library not found

#=============================================================================
# Copyright 2009 RobotCub Consortium
#   Authors: Lorenzo Natale <lorenzo.natale@iit.it>
#            Vadim Tikhanoff <vadim.tikhanoff@iit.it>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)


# Dec 2009, reorganized to handle new windows ode versions.

# find (all) header files for include directories:
if(NOT ODE_DIR)
  set(ODE_ENV_DIR $ENV{ODE_DIR})
  if(ODE_ENV_DIR)
    find_path(ODE_DIR include/ode/ode.h HINTS "${ODE_ENV_DIR}")
  else()
    find_path(ODE_DIR include/ode/ode.h ${CMAKE_PROJECT_DIR})
  endif()
endif()

find_path(ODE_INCLUDE_DIRS ode/ode.h HINTS "${ODE_DIR}/include")
mark_as_advanced(ODE_INCLUDE_DIRS)

set(ODE_SYSTEM_LIBS)
set(ODE_SYSTEM_LIBS_FOUND FALSE)
set(ODE_FOUND FALSE)

if(MSVC)
  option(ODE_STATIC "Link ODE as static library? Warning in windows, to coexist with YARP you need to set this to FALSE" TRUE)
  option(ODE_DOUBLE_PRECISION "Use double precision version of the ODE library" TRUE)

  #search static and dynamic libs, single and double precision
  find_library(ODE_MAIN_STATIC_DOUBLE_DB
               NAMES ode ode_doubled
               HINTS "${ODE_DIR}/lib/debuglib" "${ODE_DIR}/lib/debugdoublelib")

  find_library(ODE_MAIN_STATIC_DOUBLE_RE
               NAMES ode ode_double
               HINTS "${ODE_DIR}/lib/releaselib" "${ODE_DIR}/lib/releasedoublelib")

  find_library(ODE_MAIN_DLL_DOUBLE_DB
               NAMES ode ode_doubled
               HINTS "${ODE_DIR}/lib/debugdll" "${ODE_DIR}/lib/debugdoubledll")

  find_library(ODE_MAIN_DLL_DOUBLE_RE
               NAMES ode ode_double
               HINTS "${ODE_DIR}/lib/releasedll" "${ODE_DIR}/lib/releasedoubledll")

  find_library(ODE_MAIN_STATIC_SINGLE_DB
               NAMES ode ode_singled
               HINTS "${ODE_DIR}/lib/debuglib" "${ODE_DIR}/lib/debugsinglelib")

  find_library(ODE_MAIN_STATIC_SINGLE_RE
               NAMES ode ode_single
               HINTS "${ODE_DIR}/lib/releaselib" "${ODE_DIR}/lib/releasesinglelib")

  find_library(ODE_MAIN_DLL_SINGLE_DB
               NAMES ode ode_singled
               HINTS "${ODE_DIR}/lib/debugdll" "${ODE_DIR}/lib/debugsingledll")

  find_library(ODE_MAIN_DLL_SINGLE_RE
               NAMES ode ode_single
               HINTS "${ODE_DIR}/lib/releasedll" "${ODE_DIR}/lib/releasesingledll")

  mark_as_advanced(ODE_MAIN_STATIC_SINGLE_DB
                   ODE_MAIN_STATIC_SINGLE_RE
                   ODE_MAIN_STATIC_DOUBLE_DB
                   ODE_MAIN_STATIC_DOUBLE_RE
                   ODE_MAIN_DLL_DOUBLE_DB
                   ODE_MAIN_DLL_DOUBLE_RE
                   ODE_MAIN_DLL_SINGLE_DB
                   ODE_MAIN_DLL_SINGLE_RE)

  set(ODE_SYSTEM_LIBS opengl32 glu32)
  set(ODE_SYSTEM_LIBS_FOUND TRUE)

#  if(ODE_MAIN_STATIC_DB OR ODE_MAIN_LIBRARY_DB OR ODE_MAIN_STATIC_RE OR ODE_MAIN_LIBRARY_RE)
#    mark_as_advanced(CLEAR ODE_STATIC)
#  else()
#    mark_as_advanced(ODE_STATIC)
#  endif()

  if(ODE_STATIC AND NOT ODE_DOUBLE_PRECISION)
    if(ODE_MAIN_STATIC_SINGLE_RE AND ODE_MAIN_STATIC_SINGLE_DB AND ODE_SYSTEM_LIBS_FOUND AND ODE_INCLUDE_DIRS)
      set(ODE_FOUND TRUE)
      set(ODE_LIBRARIES "${ODE_SYSTEM_LIBS};optimized;${ODE_MAIN_STATIC_SINGLE_RE};debug;${ODE_MAIN_STATIC_SINGLE_DB}")
    endif()
  endif()

  if(ODE_STATIC AND ODE_DOUBLE_PRECISION)
    if(ODE_MAIN_STATIC_DOUBLE_RE AND ODE_MAIN_STATIC_DOUBLE_DB AND ODE_SYSTEM_LIBS_FOUND AND ODE_INCLUDE_DIRS)
      set(ODE_FOUND TRUE)
      set(ODE_LIBRARIES "${ODE_SYSTEM_LIBS};optimized;${ODE_MAIN_STATIC_DOUBLE_RE};debug;${ODE_MAIN_STATIC_DOUBLE_DB}")
    endif()
  endif()

  if(NOT ODE_STATIC AND NOT ODE_DOUBLE_PRECISION)
    if(ODE_MAIN_DLL_SINGLE_RE AND ODE_MAIN_DLL_SINGLE_DB AND ODE_SYSTEM_LIBS_FOUND AND ODE_INCLUDE_DIRS)
      set(ODE_FOUND TRUE)
      set(ODE_LIBRARIES "${ODE_SYSTEM_LIBS};optimized;${ODE_MAIN_DLL_SINGLE_RE};debug;${ODE_MAIN_DLL_SINGLE_DB}")
    endif()
  endif()

  if(NOT ODE_STATIC AND ODE_DOUBLE_PRECISION)
    if(ODE_MAIN_DLL_DOUBLE_RE AND ODE_MAIN_DLL_DOUBLE_DB AND ODE_SYSTEM_LIBS_FOUND AND ODE_INCLUDE_DIRS)
      set(ODE_FOUND TRUE)
      set(ODE_LIBRARIES "${ODE_SYSTEM_LIBS};optimized;${ODE_MAIN_DLL_DOUBLE_RE};debug;${ODE_MAIN_DLL_DOUBLE_DB}")
    endif()
  endif()

else()

  find_library(ODE_MAIN_LIBRARY NAMES ode libode HINTS "${ODE_DIR}/lib" "${ODE_DIR}/ode/src" "${ODE_DIR}/ode/src/.libs")
  mark_as_advanced(ODE_MAIN_LIBRARY)

  find_package(OpenGL)
  if(NOT OPENGL_FOUND)
    message(STATUS "Cannot find OpenGL, but ODE needs it")
  endif(NOT OPENGL_FOUND)
  set(ODE_SYSTEM_LIBS ${OPENGL_LIBRARIES})
  set(ODE_SYSTEM_LIBS_FOUND ${OPENGL_FOUND})

  if(ODE_MAIN_LIBRARY AND ODE_SYSTEM_LIBS_FOUND AND ODE_INCLUDE_DIRS)
    set(ODE_FOUND TRUE)
    set(ODE_LIBRARIES ${ODE_MAIN_LIBRARY} ${ODE_SYSTEM_LIBS})
    #message(STATUS "ODE libraries: ${ODE_LIBRARIES}")
  endif()

  if(NOT CMAKE_CROSS_COMPILING)
    include(CheckCXXSourceRuns)
    set(CMAKE_REQUIRED_INCLUDES ${ODE_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${ODE_LIBRARIES})
    check_cxx_source_runs("extern \"C\" int dCheckConfiguration(const char*str); \n int main() { return dCheckConfiguration(\"ODE_double_precision\")?0:1; }\n" ODE_IS_DOUBLE)
  else()
    set(ODE_IS_DOUBLE FALSE)
  endif()

  if(ODE_IS_DOUBLE)
    set(ODE_DOUBLE_PRECISION CACHE INTERNAL "true if ODE has double precision capabilities" TRUE)
  else()
    set(ODE_DOUBLE_PRECISION CACHE INTERNAL "true if ODE has double precision capabilities" FALSE)
  endif()

endif()

if(ODE_FOUND)
  if(NOT ODE_FIND_QUIETLY)
    message(STATUS "Found ODE: ${ODE_LIBRARIES}")
  endif(NOT ODE_FIND_QUIETLY)
else()
  if(ODE_FIND_REQUIRED OR ODE_DIR)
    if(ODE_DIR)
      message(STATUS "ODE library not found, check ODE_DIR")
    else()
      message(FATAL_ERROR "ODE library not found, check ODE_DIR")
    endif()
  endif()
endif()
