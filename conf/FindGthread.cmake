# Copyright: (C) 2009 RobotCub Consortium
# Authors: Alessandro Scalzo, Lorenzo Natale, Paul Fitzpatrick, Gianluca Massera
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Tried to make variables uniform with cmake "standards"
# Now set Gthread_* variables (respect capitalization rule).
# Maintain backwards compatibiliy.
# Lorenzo Natale 14/05/2010
#
# On exit set:
# Gthread_FOUND
# Gthread_LIBRARIES
# Gthread_INCLUDE_DIRS
#
# Old variables:
#
# GTHREAD_FOUND
# GTHREAD_LINK_FLAGS
# GTHREAD_INCLUDE_DIR
#
# Added: 03/11/2011, Lorenzo
# Windows: support gtkmm x64, search also using GTKMM64_BASEPATH (this variable is set by the installer).
#

INCLUDE(FindPackageHandleStandardArgs)

IF(UNIX)
 INCLUDE(FindPkgConfig)
 if (PKG_CONFIG_FOUND)
   pkg_check_modules(GTHREAD "gthread-2.0")
   if (GTHREAD_FOUND)
     set(GTHREAD_INCLUDE_DIR ${GTHREAD_INCLUDE_DIRS})
     set(GTHREAD_LIBRARY_DIR ${GTHREAD_LIBRARY_DIRS})
     set(GTHREAD_LINK_FLAGS ${GTHREAD_LDFLAGS})
     set(GTHREAD_C_FLAGS ${GTHREAD_CFLAGS})
   endif (GTHREAD_FOUND)
 endif (PKG_CONFIG_FOUND)
 
 IF (APPLE)
  IF (NOT GTHREAD_LINK_FLAGS)
    ### THERE IS AN OFFICIAL GTK PACKAGE on www.gtk-osx.org
    ### The following setup is relative to the official GTK package for mac os-x
    SET( GTHREAD_INCLUDE_DIR "/Library/Frameworks/GLib.framework/Headers" )
    SET( GTHREAD_LIBRARY_DIR " " )
    SET( GTHREAD_LINK_FLAGS "-framework GLib" )
    SET( GTHREAD_C_FLAGS " " )
  ENDIF ()
 ENDIF (APPLE)

 SET(GTHREAD_LINK_FLAGS "${GTHREAD_LINK_FLAGS}" CACHE INTERNAL "gthread link flags")
 SET(GTHREAD_C_FLAGS "${GTHREAD_C_FLAGS}" CACHE INTERNAL "gthread include flags")
 SET(GTHREAD_INCLUDE_DIR "${GTHREAD_INCLUDE_DIR}" CACHE INTERNAL "gthread include directory")

 SET(Gthread_LIBRARIES "${GTHREAD_LINK_FLAGS}" CACHE INTERNAL "gthread link flags")
 SET(Gthread_C_FLAGS "${GTHREAD_C_FLAGS}" CACHE INTERNAL "gthread include flags")
 SET(Gthread_INCLUDE_DIRS "${GTHREAD_INCLUDE_DIR}" CACHE INTERNAL "gthread include directory")

 IF (GTHREAD_C_FLAGS)
 	SET(Gthread_FOUND TRUE)
 ELSE (GTHREAD_C_FLAGS)
	SET(Gthread_FOUND FALSE)
 ENDIF (GTHREAD_C_FLAGS)
 
 FIND_PACKAGE_HANDLE_STANDARD_ARGS(Gthread "GTHREAD not found" Gthread_LIBRARIES Gthread_INCLUDE_DIRS)
ELSE (UNIX)

  # search GTKMM_BASEPATH to support gthread shipped and installed from gtkmm
  # priority to GTKMM64_BASEPATH (x64 build)
  set(GTK_BASEPATH $ENV{GTKMM64_BASEPATH})
  if (NOT GTK_BASEPATH)
      set(GTK_BASEPATH $ENV{GTKMM_BASEPATH})
  endif()
  
  if (NOT GTK_BASEPATH)
      set(GTK_BASEPATH $ENV{GTK_BASEPATH})
  endif()
	  
  FIND_LIBRARY(GTK_thread_lib
	NAMES gthread-2.0
	PATHS ${GTK_BASEPATH}/lib)
  
  SET(Gthread_INCLUDE_DIRS "" CACHE PATH "Gthread include directory")
  
  if (GTK_thread_lib)
    set(Gthread_LIBRARIES ${GTK_thread_lib} CACHE STRING "Gthread libraries")
  endif(GTK_thread_lib)
  
  find_package_handle_standard_args(Gthread DEFAULT_MSG Gthread_LIBRARIES)

ENDIF (UNIX)

mark_as_advanced(Gthread_INCLUDE_DIRS Gthread_LIBRARIES)
