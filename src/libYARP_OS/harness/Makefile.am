
TESTS = harness
check_PROGRAMS = harness

check-local: harness
	./harness regression

harness_SOURCES = \
AddressTest.cpp                   PortCoreTest.cpp \
BottleTest.cpp                    PortTest.cpp \
BufferedConnectionWriterTest.cpp  ProtocolTest.cpp \
ElectionTest.cpp                  StreamConnectionReaderTest.cpp \
harness.cpp                       StringInputStreamTest.cpp \
NameConfigTest.cpp                StringOutputStreamTest.cpp \
NameServerTest.cpp                StringTest.cpp \
NetTypeTest.cpp                   ThreadTest.cpp \
PortCommandTest.cpp               TimeTest.cpp \
TestList.h


harness_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include

