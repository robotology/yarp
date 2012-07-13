# $Id: Makefile.am 4 2008-06-22 09:19:11Z rbock $

lib_LTLIBRARIES		= libgtkdatabox.la
libgtkdatabox_la_SOURCES= \
			gtkdatabox.c\
			gtkdatabox_marshal.c\
			gtkdatabox_scale.c\
			gtkdatabox_graph.c\
			gtkdatabox_xyc_graph.c\
 			gtkdatabox_points.c\
                        gtkdatabox_lines.c\
                        gtkdatabox_bars.c\
			gtkdatabox_markers.c\
                        gtkdatabox_cross_simple.c\
                        gtkdatabox_grid.c\
			gtkdatabox_ruler.c

libgtkdatabox_la_LDFLAGS= \
		        -version-info $(LT_CURRENT)\
			-release $(LT_RELEASE)\
			@GTK_LIBS@


include_HEADERS		= \
			gtkdatabox_typedefs.h\
			gtkdatabox.h\
			gtkdatabox_marshal.h\
			gtkdatabox_scale.h\
			gtkdatabox_graph.h\
			gtkdatabox_xyc_graph.h\
 			gtkdatabox_points.h\
                        gtkdatabox_lines.h\
                        gtkdatabox_bars.h\
			gtkdatabox_markers.h\
                        gtkdatabox_cross_simple.h\
                        gtkdatabox_grid.h\
			gtkdatabox_ruler.h

EXTRA_DIST = gtkdatabox_marshal.list

BUILT_SOURCES = gtkdatabox_marshal.c gtkdatabox_marshal.h

$(srcdir)/gtkdatabox_marshal.c: $(srcdir)/gtkdatabox_marshal.list
	glib-genmarshal --body --prefix=gtk_databox_marshal --internal $< > $@

$(srcdir)/gtkdatabox_marshal.h: $(srcdir)/gtkdatabox_marshal.list
	glib-genmarshal --header --prefix=gtk_databox_marshal --internal $< > $@

INCLUDES		= \
			-I$(top_srcdir)

AM_CFLAGS			=\
			-O2\
			-W -Wall -pedantic\
			-DG_DISABLE_DEPRECATED\
			-DGDK_DISABLE_DEPRECATED\
			-DGDK_PIXBUF_DISABLE_DEPRECATED\
			-DGTK_DISABLE_DEPRECATED\
			-DGTK_MULTIHEAD_SAFE=1\
			`pkg-config gtk+-2.0 --cflags`
