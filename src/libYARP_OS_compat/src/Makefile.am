
INCLUDES = -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include

lib_LTLIBRARIES = libYARP_OS_compat.la

libYARP_OS_compat_la_SOURCES = \
YARP1.cpp           YARPParseParameters.cpp  YARPScheduler.cpp  YARPTime.cpp \
YARPBottle.cpp      YARPPort.cpp             YARPSemaphore.cpp \
YARPConfigFile.cpp  YARPRefCount.cpp         YARPThread.cpp

libYARP_OS_compat_la_LDFLAGS = -rpath $(libdir)
