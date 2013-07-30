# Copyright: (C) 2009 RobotCub Consortium
# Authors: Giorgio Metta, Lorenzo Natale, Stephen Hart
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Creates:
# GTKMM_INCLUDE_DIR   - Directories to include to use GTKMM
# GTKMM_LINK_FLAGS    - Files to link against to use GTKMM
# GTKMM_LINK_DIR      - Directories containing libraries to use GTKMM
# GtkMM_FOUND         - If false, don't try to use GTKMM
# GtkMM_VERSION_MAJOR
# GtkMM_VERSION_MINOR

# gtkmm and libglademm seem to be coupled in FindGtkMMWin32.cmake
# so mirror that behavior here

# 13/04/2011: added variable to store version (GTKMM_VERSION is copied to GtkMM_VERSION)
# Split GTKMM_VERSION into GtkMM_VERSION_MAJOR and GtkMM_VERSION_MAJOR

FIND_PACKAGE(PkgConfig)

# prerequisite
FIND_PACKAGE(GtkPlus REQUIRED)
IF (NOT GtkPlus_FOUND)
    MESSAGE(SEND_ERROR "GtkPlus was not found but GtkMM requires GtkPlus.")
ENDIF (NOT GtkPlus_FOUND)

IF (GTKPLUS_C_FLAG)
    LIST(APPEND GTKMM_C_FLAGS ${GTKPLUS_C_FLAGS})
ENDIF (GTKPLUS_C_FLAG)

if(PKG_CONFIG_FOUND)
  
  PKG_CHECK_MODULES(GTKMM gtkmm-2.4>=2.8.8)
  # PKG_CHECK_MODULES(GLADE libglademm-2.4)

# Removed: can't see the difference with respect to prev. line
# if you need gthread: use FindGthread.cmake instead.
#  PKG_CHECK_MODULES(GTHREAD libglademm-2.4)
endif(PKG_CONFIG_FOUND)

FIND_PACKAGE(Gthread)

IF (GTKMM_FOUND)
  MESSAGE(STATUS " pkg-config found gtkmm")
  SET(GtkMM_FOUND TRUE)
  SET(GtkMM_INCLUDE_DIRS ${GTKMM_INCLUDE_DIRS})
  SET(GtkMM_LIBRARY_DIRS ${GTKMM_LIBDIR})
  SET(GtkMM_LIBRARIES  ${GTKMM_LDFLAGS})
  SET(GtkMM_C_FLAGS  ${GTKMM_CFLAGS})

  if (GTKMM_gtkmm-2.4_VERSION)
        set(GtkMM_VERSION ${GTKMM_gtkmm-2.4_VERSION})
  elseif(GTKMM_VERSION)
        set(GtkMM_VERSION ${GTKMM_VERSION})
  else()
        ## if everything else fail assume 2.8.8
        set(GtkMM_VERSION 2.8.8)
  endif()

  #  message(STATUS "GtkMM_VERSION:${GtkMM_VERSION}")
  ### now break GtkMM_VERSION into MINOR and MAJOR
  string(REPLACE "." ";" GTKMM_VERSION_LIST ${GtkMM_VERSION})

  list(GET GTKMM_VERSION_LIST 0 GtkMM_VERSION_MAJOR)
  list(GET GTKMM_VERSION_LIST 1 GtkMM_VERSION_MINOR)
 
ELSE (GTKMM_FOUND)
  SET(GtkMM_FOUND FALSE)
  MESSAGE(STATUS " pkg-config could not find gtkmm")
ENDIF (GTKMM_FOUND)

IF (GLADE_FOUND)
  MESSAGE(STATUS " pkg-config found glade")
  IF (GTKMM_FOUND)
    SET(GtkMM_INCLUDE_DIRS ${GTKMM_INCLUDE_DIRS} ${GLADE_INCLUDE_DIRS})
    SET(GtkMM_LIBRARY_DIRS ${GTKMM_LIBDIR} ${GLADE_LIBDIR})
    SET(GtkMM_LIBRARIES  ${GTKMM_LDFLAGS} ${GLADE_LDFLAGS})
    SET(GtkMM_C_FLAGS  ${GTKMM_CFLAGS} ${GLADE_CFLAGS})

  ENDIF (GTKMM_FOUND)
ELSE (GLADE_FOUND)
  MESSAGE(STATUS " pkg-config could not find glade")
ENDIF (GLADE_FOUND)

#prerequisite
LIST(APPEND GTKMM_INCLUDE_DIRS ${GTKPLUS_INCLUDE_DIR})
LIST(APPEND GTKMM_LIBRARIES ${GTKPLUS_LINK_FLAGS})

IF (GTHREAD_FOUND)
  MESSAGE(STATUS " pkg-config found gthread")
  IF (GTKMM_FOUND)
    SET(GtkMM_INCLUDE_DIRS ${GTKMM_INCLUDE_DIRS} ${GTHREAD_INCLUDE_DIRS})
    SET(GtkMM_LIBRARY_DIRS ${GTKMM_LIBDIR} ${GTHREAD_LIBDIR})
    SET(GtkMM_LIBRARIES  ${GTKMM_LDFLAGS} ${GTHREAD_LDFLAGS})
    SET(GtkMM_C_FLAGS  ${GTKMM_CFLAGS} ${GTHREAD_CFLAGS})
  ENDIF (GTKMM_FOUND)
ELSE (GTHREAD_FOUND)
  MESSAGE(STATUS " pkg-config could not find gthread")
ENDIF (GTHREAD_FOUND)

