
TESTS = harness
check_PROGRAMS = harness

check-local: harness
	./harness regression

# automake_expand harness_SOURCES *.h *.cpp
harness_SOURCES = AddressTest.cpp BinPortableTest.cpp BottleTest.cpp BufferedConnectionWriterTest.cpp ElectionTest.cpp harness.cpp NameConfigTest.cpp NameServerTest.cpp NetTypeTest.cpp PortCommandTest.cpp PortCoreTest.cpp PortTest.cpp PropertyTest.cpp ProtocolTest.cpp StreamConnectionReaderTest.cpp StringInputStreamTest.cpp StringOutputStreamTest.cpp StringTest.cpp TestList.h ThreadTest.cpp TimeTest.cpp ValueTest.cpp VocabTest.cpp 


harness_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include

