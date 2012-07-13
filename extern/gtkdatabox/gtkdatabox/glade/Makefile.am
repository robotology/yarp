# $Id: Makefile.am 4 2008-06-22 09:19:11Z rbock $

if BUILD_LIBGLADE

# Build the runtime parts for libglade. 

INCLUDES = \
	-I$(top_builddir)				\
	-I$(top_srcdir)/gtk				
#	@LIBGLADE_CFLAGS@

glademoduledir = @libglade_moduledir@
glademodule_LTLIBRARIES = libdatabox.la

libdatabox_la_CFLAGS = @LIBGLADE_CFLAGS@

libdatabox_la_LIBADD = \
	$(top_builddir)/gtk/libgtkdatabox.la	\
	@LIBGLADE_LIBS@

libdatabox_la_LDFLAGS = \
	-export-dynamic -module -avoid-version 

libdatabox_la_SOURCES = \
	glade-databox.c


endif

if BUILD_GLADE

# These are glade-3 interface builder modules, only needed if 
# you are going to design with glade-3

INCLUDES = \
	-I$(top_builddir)				\
	-I$(top_srcdir)/gtk				

gladeuimoduledir = @glade_moduledir@
gladeuimodule_LTLIBRARIES = libgladedatabox.la

libgladedatabox_la_CFLAGS = @GLADE3_CFLAGS@

libgladedatabox_la_LIBADD = \
	$(top_builddir)/gtk/libgtkdatabox.la	\
	@GLADE3_LIBS@

libgladedatabox_la_LDFLAGS = \
	-export-dynamic -module -avoid-version 

libgladedatabox_la_SOURCES = \
	gladeui-databox.c

catalogsdir = @glade_catalogdir@
catalogs_DATA = gtkdatabox.xml

endif
