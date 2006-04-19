
TESTS = harness
check_PROGRAMS = harness

check-local: harness
	./harness regression

harness_SOURCES = \
TestList.h \
harness.cpp  YARPImageTest.cpp

harness_DEPENDENCIES = \
$(top_builddir)/src/libYARP_sig_compat/src/libYARP_sig_compat.la \
$(top_builddir)/src/libYARP_sig/src/libYARP_sig.la \
$(top_builddir)/src/libYARP_OS_compat/src/libYARP_OS_compat.la \
$(top_builddir)/src/libYARP_OS/src/libYARP_OS.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include -I$(srcdir)/../../libYARP_OS_compat/include -I$(srcdir)/../../libYARP_sig/include

