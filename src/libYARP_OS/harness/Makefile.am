
bin_PROGRAMS = harness

harness_SOURCES = \
AddressTest.cpp                   PortCoreTest.cpp \
BottleTest.cpp                    PortTest.cpp \
BufferedConnectionWriterTest.cpp  ProtocolTest.cpp \
ElectionTest.cpp                  StreamConnectionReaderTest.cpp \
harness.cpp                       StringInputStreamTest.cpp \
NameConfigTest.cpp                StringOutputStreamTest.cpp \
NameServerTest.cpp                StringTest.cpp \
NetTypeTest.cpp                   ThreadTest.cpp \
PortCommandTest.cpp               TimeTest.cpp


harness_DEPENDENCIES = $(top_builddir)/src/libYARP_OS/src/libYARP_OS.la

harness_LDADD = $(harness_DEPENDENCIES)

INCLUDES = -I. -I$(srcdir)/../include

