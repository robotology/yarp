#
# Searches and hopefully finds gtkmm on windows -- by alessandro.
# Assumes that the environment variable GTKMM_BASEPATH is set to the place
# where GTKMM libs have been unpacked/installed. Users that want to install 
# gtkmm manually must define GTKMM_BASEPATH accordingly.
#
# Creates:
# GTKMM_INCLUDE_DIR   - Directories to include to use GTKMM
# GTKMM_LINK_FLAGS    - Files to link against to use GTKMM
# GTKMM_C_FLAGS       - Flags to pass to the C/C++ compiler for GTKMM.
# GtkMM_FOUND         - If false, don't try to use GTKMM


# GTK header files
FIND_PATH(GTKMM_gdkglext-config_INCLUDE_PATH gdkglext-config.h $ENV{GTK_BASEPATH}/lib/gtkglext-1.0/include)
FIND_PATH(GTKMM_gtkgl_INCLUDE_PATH gtk/gtkgl.h $ENV{GTK_BASEPATH}/include/gtkglext-1.0)
FIND_PATH(GTKMM_glade_INCLUDE_PATH glade/glade.h $ENV{GTK_BASEPATH}/include/libglade-2.0)
FIND_PATH(GTKMM_gdk-config_INCLUDE_PATH gdkconfig.h $ENV{GTK_BASEPATH}/lib/gtk-2.0/include)
FIND_PATH(GTKMM_gtk_INCLUDE_PATH gtk/gtk.h $ENV{GTK_BASEPATH}/include/gtk-2.0)  
FIND_PATH(GTKMM_pango_INCLUDE_PATH pango/pango.h $ENV{GTK_BASEPATH}/include/pango-1.0)
FIND_PATH(GTKMM_atk_INCLUDE_PATH atk/atk.h $ENV{GTK_BASEPATH}/include/atk-1.0)
FIND_PATH(GTKMM_glib-config_INCLUDE_PATH glibconfig.h $ENV{GTK_BASEPATH}/lib/glib-2.0/include)
FIND_PATH(GTKMM_glib_INCLUDE_PATH glib.h $ENV{GTK_BASEPATH}/include/glib-2.0)
FIND_PATH(GTKMM_libxml_INCLUDE_PATH libxml/xmlversion.h $ENV{GTK_BASEPATH}/include/libxml2)
FIND_PATH(GTKMM_cairo_INCLUDE_PATH cairo.h $ENV{GTK_BASEPATH}/include/cairo)
FIND_PATH(GTKMM_include_INCLUDE_PATH gtk-2.0/gtk/gtk.h $ENV{GTK_BASEPATH}/include)

# GTKMM header files
FIND_PATH(GTKMM_libglademm_INCLUDE_PATH libglademm.h $ENV{GTKMM_BASEPATH}/include/libglademm-2.4/)
FIND_PATH(GTKMM_libglademm-config_INCLUDE_PATH libglademmconfig.h $ENV{GTKMM_BASEPATH}/lib/libglademm-2.4/include)
FIND_PATH(GTKMM_gtkmm-config_INCLUDE_PATH gtkmmconfig.h $ENV{GTKMM_BASEPATH}/lib/gtkmm-2.4/include)
FIND_PATH(GTKMM_gtkmm_INCLUDE_PATH gtkmm.h $ENV{GTKMM_BASEPATH}/include/gtkmm-2.4)
FIND_PATH(GTKMM_gdkmm-config_INCLUDE_PATH gdkmmconfig.h $ENV{GTKMM_BASEPATH}/lib/gdkmm-2.4/include)
FIND_PATH(GTKMM_gdkmm_INCLUDE_PATH gdkmm.h $ENV{GTKMM_BASEPATH}/include/gdkmm-2.4)
FIND_PATH(GTKMM_pangomm_INCLUDE_PATH pangomm.h $ENV{GTKMM_BASEPATH}/include/pangomm-1.4)
FIND_PATH(GTKMM_atkmm_INCLUDE_PATH atkmm.h $ENV{GTKMM_BASEPATH}/include/atkmm-1.6)
FIND_PATH(GTKMM_libxml++config_INCLUDE_PATH libxml++config.h $ENV{GTKMM_BASEPATH}/lib/libxml++-2.6/include)
FIND_PATH(GTKMM_libxml++_INCLUDE_PATH libxml++/libxml++.h $ENV{GTKMM_BASEPATH}/include/libxml++-2.6)
FIND_PATH(GTKMM_glibmm-config_INCLUDE_PATH glibmmconfig.h $ENV{GTKMM_BASEPATH}/lib/glibmm-2.4/include)
FIND_PATH(GTKMM_glibmm_INCLUDE_PATH glibmm.h $ENV{GTKMM_BASEPATH}/include/glibmm-2.4)
FIND_PATH(GTKMM_sigc++config_INCLUDE_PATH sigc++config.h $ENV{GTKMM_BASEPATH}/lib/sigc++-2.0/include)
FIND_PATH(GTKMM_sigc++_INCLUDE_PATH sigc++/sigc++.h $ENV{GTKMM_BASEPATH}/include/sigc++-2.0)

# GTK libraries
FIND_LIBRARY(GTKMM_gtk_LIBRARY NAMES gtk-win32-2.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_gdk_LIBRARY NAMES gdk-win32-2.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_gdk-pixbuf_LIBRARY NAMES gdk_pixbuf-2.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_glib_LIBRARY NAMES glib-2.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_gobject_LIBRARY NAMES gobject-2.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_glade_LIBRARY NAMES glade-2.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_xml2_LIBRARY NAMES xml2 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_atk_LIBRARY NAMES atk-1.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_pangowin32_LIBRARY NAMES pangowin32-1.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_pangocairo_LIBRARY NAMES pangocairo-1.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_pango_LIBRARY NAMES pango-1.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_cairo_LIBRARY NAMES cairo PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_gmodule_LIBRARY NAMES gmodule-2.0 PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_intl_LIBRARY NAMES intl PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_iconv_LIBRARY NAMES iconv PATHS $ENV{GTK_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_gthread_LIBRARY NAMES gthread-2.0 PATHS $ENV{GTK_BASEPATH}/lib)

# GTKMM libraries
FIND_LIBRARY(GTKMM_glademm_LIBRARY NAMES glademm-2.4 PATHS $ENV{GTKMM_BASEPATH}/lib) 
FIND_LIBRARY(GTKMM_xml++_LIBRARY NAMES xml++-2.6 PATHS $ENV{GTKMM_BASEPATH}/lib) 
FIND_LIBRARY(GTKMM_gtkmm_LIBRARY NAMES gtkmm-2.4 PATHS $ENV{GTKMM_BASEPATH}/lib) 
FIND_LIBRARY(GTKMM_gdkmm_LIBRARY NAMES gdkmm-2.4 PATHS $ENV{GTKMM_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_atkmm_LIBRARY NAMES atkmm-1.6 PATHS $ENV{GTKMM_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_pangomm_LIBRARY NAMES pangomm-1.4 PATHS $ENV{GTKMM_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_glibmm_LIBRARY NAMES glibmm-2.4 PATHS $ENV{GTKMM_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_sigc_LIBRARY NAMES sigc-2.0 PATHS $ENV{GTKMM_BASEPATH}/lib) 

FIND_LIBRARY(GTKMM_glademm_LIBRARYd NAMES glademm-2.4d PATHS $ENV{GTKMM_BASEPATH}/lib) 
FIND_LIBRARY(GTKMM_xml++_LIBRARYd NAMES xml++-2.6d PATHS $ENV{GTKMM_BASEPATH}/lib) 
FIND_LIBRARY(GTKMM_gtkmm_LIBRARYd NAMES gtkmm-2.4d PATHS $ENV{GTKMM_BASEPATH}/lib) 
FIND_LIBRARY(GTKMM_gdkmm_LIBRARYd NAMES gdkmm-2.4d PATHS $ENV{GTKMM_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_atkmm_LIBRARYd NAMES atkmm-1.6d PATHS $ENV{GTKMM_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_pangomm_LIBRARYd NAMES pangomm-1.4d PATHS $ENV{GTKMM_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_glibmm_LIBRARYd NAMES glibmm-2.4d PATHS $ENV{GTKMM_BASEPATH}/lib)
FIND_LIBRARY(GTKMM_sigc_LIBRARYd NAMES sigc-2.0d PATHS $ENV{GTKMM_BASEPATH}/lib) 
 
IF(GTKMM_gdkglext-config_INCLUDE_PATH AND
   GTKMM_gtkgl_INCLUDE_PATH AND
   GTKMM_glade_INCLUDE_PATH AND
   GTKMM_gdk-config_INCLUDE_PATH AND
   GTKMM_gtk_INCLUDE_PATH AND
   GTKMM_pango_INCLUDE_PATH AND
   GTKMM_atk_INCLUDE_PATH AND
   GTKMM_glib-config_INCLUDE_PATH AND
   GTKMM_glib_INCLUDE_PATH AND
   GTKMM_libxml_INCLUDE_PATH AND
   GTKMM_cairo_INCLUDE_PATH AND
   GTKMM_include_INCLUDE_PATH AND
   GTKMM_libglademm_INCLUDE_PATH AND 
   GTKMM_libglademm-config_INCLUDE_PATH AND 
   GTKMM_gtkmm-config_INCLUDE_PATH AND
   GTKMM_gtkmm_INCLUDE_PATH AND
   GTKMM_gdkmm-config_INCLUDE_PATH AND
   GTKMM_gdkmm_INCLUDE_PATH AND
   GTKMM_pangomm_INCLUDE_PATH AND
   GTKMM_atkmm_INCLUDE_PATH AND
   GTKMM_libxml++config_INCLUDE_PATH AND
   GTKMM_libxml++_INCLUDE_PATH AND
   GTKMM_glibmm-config_INCLUDE_PATH AND
   GTKMM_glibmm_INCLUDE_PATH AND
   GTKMM_sigc++config_INCLUDE_PATH AND
   GTKMM_sigc++_INCLUDE_PATH AND
   GTKMM_gtk_LIBRARY AND
   GTKMM_gdk_LIBRARY AND 
   GTKMM_gdk-pixbuf_LIBRARY AND 
   GTKMM_glib_LIBRARY AND 
   GTKMM_gobject_LIBRARY AND 
   GTKMM_glade_LIBRARY AND 
   GTKMM_xml2_LIBRARY AND 
   GTKMM_atk_LIBRARY AND 
   GTKMM_pangowin32_LIBRARY AND 
   GTKMM_pangocairo_LIBRARY AND 
   GTKMM_pango_LIBRARY AND 
   GTKMM_cairo_LIBRARY AND 
   GTKMM_gmodule_LIBRARY AND 
   GTKMM_intl_LIBRARY AND 
   GTKMM_iconv_LIBRARY AND
   GTKMM_glademm_LIBRARY AND 
   GTKMM_xml++_LIBRARY AND 
   GTKMM_gtkmm_LIBRARY AND 
   GTKMM_gdkmm_LIBRARY AND 
   GTKMM_atkmm_LIBRARY AND 
   GTKMM_pangomm_LIBRARY AND 
   GTKMM_glibmm_LIBRARY AND 
   GTKMM_sigc_LIBRARY AND
   GTKMM_gthread_LIBRARY)

   SET(GtkMM_FOUND TRUE)
   
   SET(GTKMM_INCLUDE_DIR ${GTKMM_gdkglext-config_INCLUDE_PATH}
       ${GTKMM_gtkgl_INCLUDE_PATH}
       ${GTKMM_glade_INCLUDE_PATH}
       ${GTKMM_gdk-config_INCLUDE_PATH}
       ${GTKMM_gtk_INCLUDE_PATH}
       ${GTKMM_pango_INCLUDE_PATH}
       ${GTKMM_atk_INCLUDE_PATH}
       ${GTKMM_glib-config_INCLUDE_PATH}
       ${GTKMM_glib_INCLUDE_PATH}
       ${GTKMM_libxml_INCLUDE_PATH}
       ${GTKMM_cairo_INCLUDE_PATH}
       ${GTKMM_include_INCLUDE_PATH}
       ${GTKMM_libglademm_INCLUDE_PATH} 
       ${GTKMM_libglademm-config_INCLUDE_PATH} 
       ${GTKMM_gtkmm-config_INCLUDE_PATH}
       ${GTKMM_gtkmm_INCLUDE_PATH}
       ${GTKMM_gdkmm-config_INCLUDE_PATH}
       ${GTKMM_gdkmm_INCLUDE_PATH}
       ${GTKMM_pangomm_INCLUDE_PATH}
       ${GTKMM_atkmm_INCLUDE_PATH}
       ${GTKMM_libxml++config_INCLUDE_PATH}
       ${GTKMM_libxml++_INCLUDE_PATH}
       ${GTKMM_glibmm-config_INCLUDE_PATH}
       ${GTKMM_glibmm_INCLUDE_PATH}
       ${GTKMM_sigc++config_INCLUDE_PATH}
       ${GTKMM_sigc++_INCLUDE_PATH})
   
   SET(GTKMM_LINK_BASE 
       ${GTKMM_gtk_LIBRARY}
       ${GTKMM_gdk_LIBRARY} 
       ${GTKMM_gdk-pixbuf_LIBRARY} 
       ${GTKMM_glib_LIBRARY} 
       ${GTKMM_gobject_LIBRARY} 
       ${GTKMM_glade_LIBRARY} 
       ${GTKMM_xml2_LIBRARY} 
       ${GTKMM_atk_LIBRARY} 
       ${GTKMM_pangowin32_LIBRARY} 
       ${GTKMM_pangocairo_LIBRARY} 
       ${GTKMM_pango_LIBRARY} 
       ${GTKMM_cairo_LIBRARY} 
       ${GTKMM_gmodule_LIBRARY} 
       ${GTKMM_intl_LIBRARY} 
       ${GTKMM_iconv_LIBRARY}
       ${GTKMM_gthread_LIBRARY})
   
    SET(GTKMM_LINK_FLAGS 
       optimized ${GTKMM_glademm_LIBRARY} 
       optimized ${GTKMM_xml++_LIBRARY} 
       optimized ${GTKMM_gtkmm_LIBRARY} 
       optimized ${GTKMM_gdkmm_LIBRARY} 
       optimized ${GTKMM_atkmm_LIBRARY} 
       optimized ${GTKMM_pangomm_LIBRARY} 
       optimized ${GTKMM_glibmm_LIBRARY} 
       optimized ${GTKMM_sigc_LIBRARY}
       debug ${GTKMM_glademm_LIBRARYd} 
       debug ${GTKMM_xml++_LIBRARYd} 
       debug ${GTKMM_gtkmm_LIBRARYd} 
       debug ${GTKMM_gdkmm_LIBRARYd} 
       debug ${GTKMM_atkmm_LIBRARYd} 
       debug ${GTKMM_pangomm_LIBRARYd} 
       debug ${GTKMM_glibmm_LIBRARYd} 
       debug ${GTKMM_sigc_LIBRARYd}
       ${GTKMM_LINK_BASE})

ELSE(GTKMM_gdkglext-config_INCLUDE_PATH AND
   GTKMM_gtkgl_INCLUDE_PATH AND
   GTKMM_glade_INCLUDE_PATH AND
   GTKMM_gdk-config_INCLUDE_PATH AND
   GTKMM_gtk_INCLUDE_PATH AND
   GTKMM_pango_INCLUDE_PATH AND
   GTKMM_atk_INCLUDE_PATH AND
   GTKMM_glib-config_INCLUDE_PATH AND
   GTKMM_glib_INCLUDE_PATH AND
   GTKMM_libxml_INCLUDE_PATH AND
   GTKMM_cairo_INCLUDE_PATH AND
   GTKMM_include_INCLUDE_PATH AND
   GTKMM_libglademm_INCLUDE_PATH AND 
   GTKMM_libglademm-config_INCLUDE_PATH AND 
   GTKMM_gtkmm-config_INCLUDE_PATH AND
   GTKMM_gtkmm_INCLUDE_PATH AND
   GTKMM_gdkmm-config_INCLUDE_PATH AND
   GTKMM_gdkmm_INCLUDE_PATH AND
   GTKMM_pangomm_INCLUDE_PATH AND
   GTKMM_atkmm_INCLUDE_PATH AND
   GTKMM_libxml++config_INCLUDE_PATH AND
   GTKMM_libxml++_INCLUDE_PATH AND
   GTKMM_glibmm-config_INCLUDE_PATH AND
   GTKMM_glibmm_INCLUDE_PATH AND
   GTKMM_sigc++config_INCLUDE_PATH AND
   GTKMM_sigc++_INCLUDE_PATH AND
   GTKMM_gtk_LIBRARY AND
   GTKMM_gdk_LIBRARY AND 
   GTKMM_gdk-pixbuf_LIBRARY AND 
   GTKMM_glib_LIBRARY AND 
   GTKMM_gobject_LIBRARY AND 
   GTKMM_glade_LIBRARY AND 
   GTKMM_xml2_LIBRARY AND 
   GTKMM_atk_LIBRARY AND 
   GTKMM_pangowin32_LIBRARY AND 
   GTKMM_pangocairo_LIBRARY AND 
   GTKMM_pango_LIBRARY AND 
   GTKMM_cairo_LIBRARY AND 
   GTKMM_gmodule_LIBRARY AND 
   GTKMM_intl_LIBRARY AND 
   GTKMM_iconv_LIBRARY AND
   GTKMM_glademm_LIBRARY AND 
   GTKMM_xml++_LIBRARY AND 
   GTKMM_gtkmm_LIBRARY AND 
   GTKMM_gdkmm_LIBRARY AND 
   GTKMM_atkmm_LIBRARY AND 
   GTKMM_pangomm_LIBRARY AND 
   GTKMM_glibmm_LIBRARY AND 
   GTKMM_sigc_LIBRARY AND
   GTKMM_gthread_LIBRARY)

	SET(GtkMM_FOUND FALSE)

ENDIF(GTKMM_gdkglext-config_INCLUDE_PATH AND
   GTKMM_gtkgl_INCLUDE_PATH AND
   GTKMM_glade_INCLUDE_PATH AND
   GTKMM_gdk-config_INCLUDE_PATH AND
   GTKMM_gtk_INCLUDE_PATH AND
   GTKMM_pango_INCLUDE_PATH AND
   GTKMM_atk_INCLUDE_PATH AND
   GTKMM_glib-config_INCLUDE_PATH AND
   GTKMM_glib_INCLUDE_PATH AND
   GTKMM_libxml_INCLUDE_PATH AND
   GTKMM_cairo_INCLUDE_PATH AND
   GTKMM_include_INCLUDE_PATH AND
   GTKMM_libglademm_INCLUDE_PATH AND 
   GTKMM_libglademm-config_INCLUDE_PATH AND 
   GTKMM_gtkmm-config_INCLUDE_PATH AND
   GTKMM_gtkmm_INCLUDE_PATH AND
   GTKMM_gdkmm-config_INCLUDE_PATH AND
   GTKMM_gdkmm_INCLUDE_PATH AND
   GTKMM_pangomm_INCLUDE_PATH AND
   GTKMM_atkmm_INCLUDE_PATH AND
   GTKMM_libxml++config_INCLUDE_PATH AND
   GTKMM_libxml++_INCLUDE_PATH AND
   GTKMM_glibmm-config_INCLUDE_PATH AND
   GTKMM_glibmm_INCLUDE_PATH AND
   GTKMM_sigc++config_INCLUDE_PATH AND
   GTKMM_sigc++_INCLUDE_PATH AND
   GTKMM_gtk_LIBRARY AND
   GTKMM_gdk_LIBRARY AND 
   GTKMM_gdk-pixbuf_LIBRARY AND 
   GTKMM_glib_LIBRARY AND 
   GTKMM_gobject_LIBRARY AND 
   GTKMM_glade_LIBRARY AND 
   GTKMM_xml2_LIBRARY AND 
   GTKMM_atk_LIBRARY AND 
   GTKMM_pangowin32_LIBRARY AND 
   GTKMM_pangocairo_LIBRARY AND 
   GTKMM_pango_LIBRARY AND 
   GTKMM_cairo_LIBRARY AND 
   GTKMM_gmodule_LIBRARY AND 
   GTKMM_intl_LIBRARY AND 
   GTKMM_iconv_LIBRARY AND
   GTKMM_glademm_LIBRARY AND 
   GTKMM_xml++_LIBRARY AND 
   GTKMM_gtkmm_LIBRARY AND 
   GTKMM_gdkmm_LIBRARY AND 
   GTKMM_atkmm_LIBRARY AND 
   GTKMM_pangomm_LIBRARY AND 
   GTKMM_glibmm_LIBRARY AND 
   GTKMM_sigc_LIBRARY AND
   GTKMM_gthread_LIBRARY)
