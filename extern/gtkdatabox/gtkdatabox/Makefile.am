# $Id: Makefile.am 4 2008-06-22 09:19:11Z rbock $

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gtkdatabox.pc

SUBDIRS = gtk glade examples docs

EXTRA_DIST = m4 depcomp scripts gtkdatabox.pc.in gtkdatabox.spec.in glade/gtkdatabox.xml

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc
