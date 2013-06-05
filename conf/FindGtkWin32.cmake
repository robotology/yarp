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

# Added: 03/11/2011, Lorenzo
# Support gtkplus from gtkmm x64, search also using GTKMM64_BASEPATH (this variable is set by the installer).
#

# first check GTKMM64
set(GTK_BASEPATH $ENV{GTKMM64_BASEPATH})

# if that failed check GTKMM_BASEPATH
if (NOT GTK_BASEPATH)
    set(GTK_BASEPATH $ENV{GTKMM_BASEPATH})
endif()

# if everything failed search GTK_BASEPATH
if (NOT GTK_BASEPATH)
    set(GTK_BASEPATH $ENV{GTK_BASEPATH})
endif()

# header files
find_path(GTK_gtk_INCLUDE_PATH gtk/gtk.h ${GTK_BASEPATH}/include/gtk-2.0/)
find_path(GTK_cairo_INCLUDE_PATH cairo.h ${GTK_BASEPATH}/include/cairo/)
find_path(GTK_glib_INCLUDE_PATH glib.h ${GTK_BASEPATH}/include/glib-2.0/)
find_path(GTK_pango_INCLUDE_PATH pango/pango.h ${GTK_BASEPATH}/include/pango-1.0/)
find_path(GTK_atk_INCLUDE_PATH atk/atk.h ${GTK_BASEPATH}/include/atk-1.0/)
find_path(GTK_glib_config_INCLUDE_PATH glibconfig.h ${GTK_BASEPATH}/lib/glib-2.0/include)
find_path(GTK_gdk_config_INCLUDE_PATH gdkconfig.h ${GTK_BASEPATH}/lib/gtk-2.0/include)
find_path(GTK_gdk_pixbuf_INCLUDE_PATH gdk-pixbuf/gdk-pixbuf.h ${GTK_BASEPATH}/include/gdk-pixbuf-2.0)
find_path(GTK_intl_INCLUDE_PATH libintl.h ${GTK_BASEPATH}/include/)

# libraries
find_library(GTK_gtk_LIBRARY
  NAMES  gtk-win32-2.0
  PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_gdk_LIBRARY
 NAMES gdk-win32-2.0
 PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_gdk_pixbuf_LIBRARY
  NAMES gdk_pixbuf-2.0
  PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_glib_LIBRARY
  NAMES glib-2.0
  PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_gobject_LIBRARY
  NAMES gobject-2.0
  PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_cairo_LIBRARY
  NAMES cairo
  PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_pango_LIBRARY
  NAMES pango-1.0
  PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_pangocairo_LIBRARY
  NAMES pangocairo-1.0
  PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_atk_LIBRARY
  NAMES atk-1.0
  PATHS ${GTK_BASEPATH}/lib)

find_library(GTK_intl_LIBRARY
  NAMES  intl
  PATHS ${GTK_BASEPATH}/lib)

# GTK_INCLUDE_DIR   - Directories to include to use GTK
# GTK_LINK_FLAGS    - Files to link against to use GTK
# GTK_FOUND         - If false, don't try to use GTK
# GTK_GL_FOUND      - If false, don't try to use GTK's GL features

if(GTK_gtk_INCLUDE_PATH AND GTK_gtk_LIBRARY
    AND GTK_glib_INCLUDE_PATH AND GTK_glib_config_INCLUDE_PATH AND GTK_glib_LIBRARY
    AND GTK_gobject_LIBRARY
    AND GTK_gdk_config_INCLUDE_PATH AND GTK_gdk_LIBRARY
    AND GTK_gdk_pixbuf_INCLUDE_PATH AND GTK_gdk_pixbuf_LIBRARY
    AND GTK_cairo_INCLUDE_PATH AND GTK_cairo_LIBRARY
    AND GTK_pango_INCLUDE_PATH AND GTK_pango_LIBRARY AND GTK_pangocairo_LIBRARY
    AND GTK_atk_INCLUDE_PATH AND GTK_atk_LIBRARY
    AND GTK_intl_INCLUDE_PATH AND GTK_intl_LIBRARY)
    set(GTK_FOUND TRUE)
    set(GTKPLUS_INCLUDE_DIR ${GTK_gtk_INCLUDE_PATH}
                            ${GTK_glib_INCLUDE_PATH}
                            ${GTK_glib_config_INCLUDE_PATH}
                            ${GTK_gdk_config_INCLUDE_PATH}
                            ${GTK_gdk_pixbuf_INCLUDE_PATH}
                            ${GTK_cairo_INCLUDE_PATH}
                            ${GTK_pango_INCLUDE_PATH}
                            ${GTK_atk_INCLUDE_PATH}
                            ${GTK_intl_INCLUDE_PATH})
    set(GTKPLUS_LINK_FLAGS ${GTK_gtk_LIBRARY}
                           ${GTK_glib_LIBRARY}
                           ${GTK_gobject_LIBRARY}
                           ${GTK_gdk_LIBRARY}
                           ${GTK_gdk_pixbuf_LIBRARY}
                           ${GTK_cairo_LIBRARY}
                           ${GTK_pango_LIBRARY}
                           ${GTK_pangocairo_LIBRARY}
                           ${GTK_atk_LIBRARY}
                           ${GTK_intl_LIBRARY})
endif()

if (GTK_gdk_pixbuf_INCLUDE_PATH)
    set(GTKPLUS_INCLUDE_DIR ${GTKPLUS_INCLUDE_DIR} ${GTK_gdk_pixbuf_INCLUDE_PATH})
endif()
