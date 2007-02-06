
bin_PROGRAMS = yarp

man_MANS = yarp.1

yarp_SOURCES = yarp.cpp

yarp_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la

yarp_LDADD = $(yarp_DEPENDENCIES)

INCLUDES = -I$(srcdir)/../include

#EXTRA_DIST = yarp.sgml
#yarp.1: yarp.sgml
#	( docbook-to-man yarp.sgml > $@ ) | ( echo "no man pages on this platform" > $@ )
#DISTCLEANFILES = yarp.1
