
TESTS = harness
check_PROGRAMS = harness

check-local: harness
	./harness regression

# automake_expand harness_SOURCES *.h *.cpp
harness_SOURCES = harness.cpp ImageTest.cpp MatrixTest.cpp SoundTest.cpp TestList.h VectorTest.cpp 


harness_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la $(top_builddir)/src/libYARP_sig/src/libYARP_sig.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include

