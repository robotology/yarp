
INCLUDES = -I$(srcdir)/../include

lib_LTLIBRARIES = libYARP_OS.la

libYARP_OS_la_SOURCES = \
Address.cpp             Logger.cpp             PortReaderBuffer.cpp \
Bottle.cpp              McastCarrier.cpp       Protocol.cpp \
BottleImpl.cpp          NameClient.cpp         Semaphore.cpp \
Carriers.cpp            NameConfig.cpp         ShmemTwoWayStream.cpp \
Companion.cpp           Name.cpp               SocketTwoWayStream.cpp \
ConstString.cpp         NameServer.cpp         StreamConnectionReader.cpp \
Contact.cpp             NetType.cpp            TcpFace.cpp \
DgramTwoWayStream.cpp   Network.cpp            Thread.cpp \
Dispatcher.cpp          PortCommand.cpp        ThreadImpl.cpp \
Election.cpp            PortCore.cpp           Time.cpp \
FakeFace.cpp            PortCoreInputUnit.cpp  UnitTest.cpp \
FallbackNameClient.cpp  PortCoreOuputUnit.cpp \
FallbackNameServer.cpp  Port.cpp

libYARP_OS_la_LDFLAGS = -rpath $(libdir)
