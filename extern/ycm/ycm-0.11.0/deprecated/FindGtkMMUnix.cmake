#.rst:
# FindGtkMMUnix (Replaced by :cmake:module:`FindGTK2`)
# ----------------------------------------------------
#
# .. warning:: This module is deprecated. You should use :cmake:module:`FindGTK2` instead.
#
# Creates::
#
#  GTKMM_INCLUDE_DIR   - Directories to include to use GTKMM
#  GTKMM_LINK_FLAGS    - Files to link against to use GTKMM
#  GTKMM_LINK_DIR      - Directories containing libraries to use GTKMM
#  GtkMM_FOUND         - If false, don't try to use GTKMM
#  GtkMM_VERSION_MAJOR
#  GtkMM_VERSION_MINOR

#=============================================================================
# Copyright 2009 RobotCub Consortium
#   Authors: Giorgio Metta <giorgio.metta@iit.it>
#            Lorenzo Natale <lorenzo.natale@iit.it>
#            Stephen Hart <stephen.hart@nasa.gov>
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


# gtkmm and libglademm seem to be coupled in FindGtkMMWin32.cmake
# so mirror that behavior here

# 13/04/2011: added variable to store version (GTKMM_VERSION is copied to GtkMM_VERSION)
# Split GTKMM_VERSION into GtkMM_VERSION_MAJOR and GtkMM_VERSION_MAJOR

include(${CMAKE_CURRENT_list_DIR}/YCMDeprecatedWarning.cmake)
ycm_deprecated_warning("FindGtkMMUnix.cmake is deprecated. Use FindGTK2 from CMake instead.")

if(NOT YCM_NO_DEPRECATED)


find_package(PkgConfig)

# prerequisite
find_package(GtkPlus REQUIRED)
if(NOT GtkPlus_FOUND)
    message(SEND_ERROR "GtkPlus was not found but GtkMM requires GtkPlus.")
endif()

if(GTKPLUS_C_FLAG)
    list(APPEND GTKMM_C_FLAGS ${GTKPLUS_C_FLAGS})
endif()

if(PKG_CONFIG_FOUND)

  PKG_CHECK_MODULES(GTKMM gtkmm-2.4>=2.8.8)
  # PKG_CHECK_MODULES(GLADE libglademm-2.4)

# Removed: can't see the difference with respect to prev. line
# if you need gthread: use FindGthread.cmake instead.
#  PKG_CHECK_MODULES(GTHREAD libglademm-2.4)
endif()

find_package(Gthread)

if(GTKMM_FOUND)
    message(STATUS " pkg-config found gtkmm")
    set(GtkMM_FOUND TRUE)
    set(GtkMM_INCLUDE_DIRS ${GTKMM_INCLUDE_DIRS})
    set(GtkMM_LIBRARY_DIRS ${GTKMM_LIBDIR})
    set(GtkMM_LIBRARIES  ${GTKMM_LDFLAGS})
    set(GtkMM_C_FLAGS  ${GTKMM_CFLAGS})

    if(GTKMM_gtkmm-2.4_VERSION)
        set(GtkMM_VERSION ${GTKMM_gtkmm-2.4_VERSION})
    elseif(GTKMM_VERSION)
        set(GtkMM_VERSION ${GTKMM_VERSION})
    else()
        ## if everything else fail assume 2.8.8
        set(GtkMM_VERSION 2.8.8)
    endif()

    #  message(STATUS "GtkMM_VERSION:${GtkMM_VERSION}")
    ### now break GtkMM_VERSION into MINOR and MAJOR
    string(REPLACE "." ";" GTKMM_VERSION_list ${GtkMM_VERSION})

    list(GET GTKMM_VERSION_list 0 GtkMM_VERSION_MAJOR)
    list(GET GTKMM_VERSION_list 1 GtkMM_VERSION_MINOR)
else()
    set(GtkMM_FOUND FALSE)
    message(STATUS " pkg-config could not find gtkmm")
endif()

if(GLADE_FOUND)
    message(STATUS " pkg-config found glade")
    if(GTKMM_FOUND)
        set(GtkMM_INCLUDE_DIRS ${GTKMM_INCLUDE_DIRS} ${GLADE_INCLUDE_DIRS})
        set(GtkMM_LIBRARY_DIRS ${GTKMM_LIBDIR} ${GLADE_LIBDIR})
        set(GtkMM_LIBRARIES  ${GTKMM_LDFLAGS} ${GLADE_LDFLAGS})
        set(GtkMM_C_FLAGS  ${GTKMM_CFLAGS} ${GLADE_CFLAGS})
    endif()
else()
    message(STATUS " pkg-config could not find glade")
endif()

# prerequisite
list(APPEND GTKMM_INCLUDE_DIRS ${GTKPLUS_INCLUDE_DIR})
list(APPEND GTKMM_LIBRARIES ${GTKPLUS_LINK_FLAGS})

if(GTHREAD_FOUND)
    message(STATUS " pkg-config found gthread")
    if(GTKMM_FOUND)
        set(GtkMM_INCLUDE_DIRS ${GTKMM_INCLUDE_DIRS} ${GTHREAD_INCLUDE_DIRS})
        set(GtkMM_LIBRARY_DIRS ${GTKMM_LIBDIR} ${GTHREAD_LIBDIR})
        set(GtkMM_LIBRARIES  ${GTKMM_LDFLAGS} ${GTHREAD_LDFLAGS})
        set(GtkMM_C_FLAGS  ${GTKMM_CFLAGS} ${GTHREAD_CFLAGS})
    endif(GTKMM_FOUND)
else(GTHREAD_FOUND)
    message(STATUS " pkg-config could not find gthread")
endif(GTHREAD_FOUND)


endif(NOT YCM_NO_DEPRECATED)
