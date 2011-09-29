#
# Searches and hopefully finds gtk on windows -- by nat.

# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# Assumes that the environment variable GTK_BASEPATH is set to the place
# where GTK libs have been unpacked/installed. This is the default 
# behavior of glade for win32, users that want to install gtk manually
# must define GTK_BASEPATH accordingly.
#
# Creates:
# GTKMM_INCLUDE_DIR   - Directories to include to use GTK
# GTKMM_LIBRARIES     - Files to link against to use GTK
# GTKMM_FOUND         - If false, don't try to use GTK
# GTKMM_GL_FOUND      - If false, don't try to use GTK's GL features

FIND_PACKAGE(GtkWin32)

# header files
FIND_PATH(GTKMM_gtkmm_INCLUDE_PATH gtkmm.h $ENV{GTK_BASEPATH}/include/gtkmm-2.4)
FIND_PATH(GTKMM_glibmm_INCLUDE_PATH glibmm.h $ENV{GTK_BASEPATH}/include/glibmm-2.4)
FIND_PATH(GTKMM_gdkmm_INCLUDE_PATH gdkmm.h $ENV{GTK_BASEPATH}/include/gdkmm-2.4)
FIND_PATH(GTKMM_cairomm_INCLUDE_PATH cairomm/cairomm.h $ENV{GTK_BASEPATH}/include/cairomm-1.0)
FIND_PATH(GTKMM_sigc_INCLUDE_PATH sigc++/sigc++.h $ENV{GTK_BASEPATH}/include/sigc++-2.0)
FIND_PATH(GTKMM_atkmm_INCLUDE_PATH atkmm.h $ENV{GTK_BASEPATH}/include/atkmm-1.6)
FIND_PATH(GTKMM_giomm_INCLUDE_PATH giomm.h $ENV{GTK_BASEPATH}/include/giomm-2.4)
FIND_PATH(GTKMM_pangomm_INCLUDE_PATH pangomm.h $ENV{GTK_BASEPATH}/include/pangomm-1.4)

FIND_PATH(GTKMM_gtkmm-config_INCLUDE_PATH gtkmmconfig.h $ENV{GTK_BASEPATH}/lib/gtkmm-2.4/include)
FIND_PATH(GTKMM_gdkmm-config_INCLUDE_PATH gdkmmconfig.h $ENV{GTK_BASEPATH}/lib/gdkmm-2.4/include)
FIND_PATH(GTKMM_glibmm-config_INCLUDE_PATH glibmmconfig.h $ENV{GTK_BASEPATH}/lib/glibmm-2.4/include)
FIND_PATH(GTKMM_sigc-config_INCLUDE_PATH sigc++config.h $ENV{GTK_BASEPATH}/lib/sigc++-2.0/include)
FIND_PATH(GTKMM_giomm-config_INCLUDE_PATH giommconfig.h $ENV{GTK_BASEPATH}/lib/giomm-2.4/include)

# libraries

FIND_LIBRARY( GTKMM_gtkmm_LIBRARY
  NAMES  gtkmm-vc90-2_4
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY( GTKMM_glibmm_LIBRARY
  NAMES  glibmm-vc90-2_4
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY( GTKMM_gdkmm_LIBRARY
  NAMES gdkmm-vc90-2_4 
  PATHS $ENV{GTK_BASEPATH}/lib)
 
FIND_LIBRARY( GTKMM_cairomm_LIBRARY
  NAMES  cairomm-vc90-1_0
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY( GTKMM_atkmm_LIBRARY
  NAMES  atkmm-vc90-1_6
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY( GTKMM_giomm_LIBRARY
  NAMES  giomm-vc90-d-2_4
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY( GTKMM_pangomm_LIBRARY
  NAMES  pangomm-vc90-d-1_4
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY( GTKMM_sigc_LIBRARY
  NAMES  sigc-vc90-2_0
  PATHS $ENV{GTK_BASEPATH}/lib)

# GTKMM_INCLUDE_DIR   - Directories to include to use GTK
# GTKMM_LINK_FLAGS    - Files to link against to use GTK
# GTKMM_FOUND         - If false, don't try to use GTK
# GTKMM_GL_FOUND      - If false, don't try to use GTK's GL features
# check only gtk include and library... lazy behavior
IF(GTKMM_gtkmm_INCLUDE_PATH AND GTKMM_cairomm_INCLUDE_PATH
	AND GTKMM_glibmm_INCLUDE_PATH AND GTKMM_gdkmm_INCLUDE_PATH	
	AND GTKMM_glibmm-config_INCLUDE_PATH AND GTKMM_gdkmm-config_INCLUDE_PATH
	AND GTKMM_sigc_INCLUDE_PATH AND GTKMM_sigc-config_INCLUDE_PATH
	AND GTKMM_atkmm_INCLUDE_PATH AND GTKMM_giomm_INCLUDE_PATH
	AND GTKMM_pangomm_INCLUDE_PATH AND GTKMM_gtkmm-config_INCLUDE_PATH 
	AND GTKMM_giomm-config_INCLUDE_PATH	
	
	AND GTKMM_gtkmm_LIBRARY AND GTKMM_glibmm_LIBRARY
	AND GTKMM_cairomm_LIBRARY AND GTKMM_gdkmm_LIBRARY
	AND GTKMM_atkmm_LIBRARY AND GTKMM_pangomm_LIBRARY
	AND GTKMM_giomm_LIBRARY AND GTKMM_sigc_LIBRARY)

	SET(GtKMM_FOUND TRUE)
	
	SET(GTKMM_INCLUDE_DIR ${GTKMM_gtkmm_INCLUDE_PATH} 
	  ${GTKMM_cairomm_INCLUDE_PATH}
	  ${GTKMM_glibmm_INCLUDE_PATH}
	  ${GTKMM_gdkmm_INCLUDE_PATH}
	  ${GTKMM_glibmm-config_INCLUDE_PATH}
	  ${GTKMM_gdkmm-config_INCLUDE_PATH}
	  ${GTKMM_sigc_INCLUDE_PATH}
	  ${GTKMM_sigc-config_INCLUDE_PATH}
	  ${GTKMM_pangomm_INCLUDE_PATH}
	  ${GTKMM_atkmm_INCLUDE_PATH}
	  ${GTKMM_giomm_INCLUDE_PATH}
	  ${GTKMM_gtkmm-config_INCLUDE_PATH}
	  ${GTKMM_giomm-config_INCLUDE_PATH}
	  ${GTKPLUS_INCLUDE_DIR})
  
  SET(GTKMM_LINK_FLAGS ${GTKMM_gtkmm_LIBRARY}
	  ${GTKMM_gdkmm_LIBRARY}
	  ${GTKMM_glibmm_LIBRARY}
	  ${GTKMM_cairomm_LIBRARY}
	  ${GTKMM_pangomm_LIBRARY}
	  ${GTKMM_atkmm_LIBRARY}
	  ${GTKMM_giomm_LIBRARY}
	  ${GTKMM_sigc_LIBRARY}
	  ${GTKPLUS_LINK_FLAGS})
 
ENDIF()

