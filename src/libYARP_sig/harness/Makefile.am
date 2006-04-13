
bin_PROGRAMS = harness

harness_SOURCES = \
harness.cpp  ImageTest.cpp


harness_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la $(top_builddir)/src/libYARP_sig/src/libYARP_sig.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include

