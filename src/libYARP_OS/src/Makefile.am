
INCLUDES = -I$(srcdir)/../include

lib_LTLIBRARIES = libYARP_OS.la

libYARP_OS_la_SOURCES = \
Address.cpp                   Logger.cpp             PortWriterBuffer.cpp \
Bottle.cpp                    McastCarrier.cpp       Property.cpp \
BottleImpl.cpp                NameClient.cpp         Protocol.cpp \
BufferedConnectionWriter.cpp  NameConfig.cpp         Random.cpp \
Carriers.cpp                  Name.cpp               Semaphore.cpp \
Companion.cpp                 NameServer.cpp         ShmemTwoWayStream.cpp \
ConstString.cpp               NetType.cpp            SocketTwoWayStream.cpp \
Contact.cpp                   Network.cpp          StreamConnectionReader.cpp \
DgramTwoWayStream.cpp         PortCommand.cpp        TcpFace.cpp \
Dispatcher.cpp                PortCore.cpp           Thread.cpp \
Election.cpp                  PortCoreInputUnit.cpp  ThreadImpl.cpp \
FakeFace.cpp                  PortCoreOuputUnit.cpp  Time.cpp \
FallbackNameClient.cpp        Port.cpp               UnitTest.cpp \
FallbackNameServer.cpp        PortReaderBuffer.cpp   Vocab.cpp

libYARP_OS_la_LDFLAGS = -rpath $(libdir)
