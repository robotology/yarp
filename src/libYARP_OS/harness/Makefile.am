
TESTS = harness
check_PROGRAMS = harness

check-local: harness
	./harness regression

harness_SOURCES = \
TestList.h \
AddressTest.cpp                   PortTest.cpp \
BinPortableTest.cpp               PropertyTest.cpp \
BottleTest.cpp                    ProtocolTest.cpp \
BufferedConnectionWriterTest.cpp  StreamConnectionReaderTest.cpp \
ElectionTest.cpp                  StringInputStreamTest.cpp \
harness.cpp                       StringOutputStreamTest.cpp \
NameConfigTest.cpp                StringTest.cpp \
NameServerTest.cpp                ThreadTest.cpp \
NetTypeTest.cpp                   TimeTest.cpp \
PortCommandTest.cpp               VocabTest.cpp \
PortCoreTest.cpp


harness_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include

