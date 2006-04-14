
bin_PROGRAMS = harness

harness_SOURCES = \
TestList.h \
harness.cpp  HeaderCompatibilityTest.cpp

harness_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la $(top_builddir)/src/libYARP_OS_compat/src/libYARP_OS_compat.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include

