
INCLUDES = -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include -I$(srcdir)/../../libYARP_OS_compat/include -I$(srcdir)/../../libYARP_sig/include

lib_LTLIBRARIES = libYARP_sig_compat.la

libYARP_sig_compat_la_SOURCES = \
cvbase.cpp         YARPImageUtils.cpp  YARPRobotMath.cpp \
YARPImage.cpp      YARPLU.cpp          YARPSimpleOperations.cpp \
YARPImageFile.cpp  YARPMatrix.cpp      YARPSVD.cpp

libYARP_sig_compat_la_LDFLAGS = -rpath $(libdir)
