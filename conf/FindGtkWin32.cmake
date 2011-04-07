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
# GTK_INCLUDE_DIR   - Directories to include to use GTK
# GTK_LIBRARIES     - Files to link against to use GTK
# GTK_FOUND         - If false, don't try to use GTK
# GTK_GL_FOUND      - If false, don't try to use GTK's GL features

# header files
FIND_PATH(GTK_gtk_INCLUDE_PATH gtk/gtk.h $ENV{GTK_BASEPATH}/include/gtk-2.0/)
FIND_PATH(GTK_cairo_INCLUDE_PATH cairo.h $ENV{GTK_BASEPATH}/include/cairo/)
FIND_PATH(GTK_glib_INCLUDE_PATH glib.h $ENV{GTK_BASEPATH}/include/glib-2.0/)
FIND_PATH(GTK_pango_INCLUDE_PATH pango/pango.h $ENV{GTK_BASEPATH}/include/pango-1.0/)
FIND_PATH(GTK_atk_INCLUDE_PATH atk/atk.h $ENV{GTK_BASEPATH}/include/atk-1.0/)
FIND_PATH(GTK_glib-config_INCLUDE_PATH glibconfig.h $ENV{GTK_BASEPATH}/lib/glib-2.0/include)
FIND_PATH(GTK_gdk-config_INCLUDE_PATH gdkconfig.h $ENV{GTK_BASEPATH}/lib/gtk-2.0/include)
FIND_PATH(GTK_gdk-pixbuf_INCLUDE_PATH gdk-pixbuf/gdk-pixbuf.h $ENV{GTK_BASEPATH}/include/gdk-pixbuf-2.0)

# libraries
FIND_LIBRARY( GTK_gtk_LIBRARY
  NAMES  gtk-win32-2.0
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY(GTK_gdk_LIBRARY
 NAMES gdk-win32-2.0
 PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY(GTK_gdk-pixbuf_LIBRARY
  NAMES gdk_pixbuf-2.0
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY(GTK_glib_LIBRARY
  NAMES glib-2.0
  PATHS $ENV{GTK_BASEPATH}/lib)

FIND_LIBRARY(GTK_gobject_LIBRARY
  NAMES gobject-2.0
  PATHS $ENV{GTK_BASEPATH}/lib)

# GTK_INCLUDE_DIR   - Directories to include to use GTK
# GTK_LINK_FLAGS    - Files to link against to use GTK
# GTK_FOUND         - If false, don't try to use GTK
# GTK_GL_FOUND      - If false, don't try to use GTK's GL features

# check only gtk include and library... lazy behavior
IF(GTK_gtk_INCLUDE_PATH AND GTK_cairo_INCLUDE_PATH
	AND GTK_glib_INCLUDE_PATH AND GTK_pango_INCLUDE_PATH
	AND GTK_atk_INCLUDE_PATH AND GTK_glib-config_INCLUDE_PATH
	AND GTK_gdk-config_INCLUDE_PATH AND GTK_gtk_LIBRARY
	AND GTK_gdk_LIBRARY AND GTK_glib_LIBRARY
	AND GTK_gdk-pixbuf_LIBRARY AND GTK_gobject_LIBRARY)
    SET(GtkPlus_FOUND TRUE)
    SET(GTKPLUS_INCLUDE_DIR ${GTK_gtk_INCLUDE_PATH} 
	  ${GTK_cairo_INCLUDE_PATH}
	  ${GTK_glib_INCLUDE_PATH}
	  ${GTK_pango_INCLUDE_PATH}
	  ${GTK_atk_INCLUDE_PATH}
	  ${GTK_glib-config_INCLUDE_PATH}
	  ${GTK_gdk-config_INCLUDE_PATH})
    SET( GTKPLUS_LINK_FLAGS  ${GTK_gtk_LIBRARY}
	  ${GTK_gdk_LIBRARY}
	  ${GTK_glib_LIBRARY}
	  ${GTK_gdk-pixbuf_LIBRARY}
	  ${GTK_gobject_LIBRARY})
ENDIF()

if (GTK_gdk-pixbuf_INCLUDE_PATH)
    set(GTKPLUS_INCLUDE_DIR ${GTKPLUS_INCLUDE_DIR} ${GTK_gdk-pixbuf_INCLUDE_PATH})
endif()