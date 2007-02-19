
TESTS = harness
check_PROGRAMS = harness

check-local: harness
	./harness --device test_grabber

# automake_expand harness_SOURCES *.h *.cpp
harness_SOURCES = harness.cpp PolyDriverTest.cpp TestList.h 


harness_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la $(top_builddir)/src/libYARP_sig/src/libYARP_sig.la $(top_builddir)/src/libYARP_dev/src/libYARP_dev.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include -I$(srcdir)/../../libYARP_sig/include

