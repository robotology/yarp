
INCLUDES = -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include -I$(srcdir)/../../libYARP_OS_compat/include -I$(srcdir)/../../libYARP_sig/include

lib_LTLIBRARIES = libYARP_sig_compat.la

libYARP_sig_compat_la_SOURCES = \
cvbase.cpp         YARPImageUtils.cpp  YARPRobotMath.cpp \
YARPImage.cpp      YARPLU.cpp          YARPSimpleOperations.cpp \
YARPImageFile.cpp  YARPMatrix.cpp      YARPSVD.cpp \
../include/yarp/cvbase.h \
../include/yarp/YARPFilters.h \
../include/yarp/YARPImageDraw.h \
../include/yarp/YARPImageFile.h \
../include/yarp/YARPImage.h \
../include/yarp/YARPImagePortContent.h \
../include/yarp/YARPImagePort.h \
../include/yarp/YARPImages.h \
../include/yarp/YARPImageUtils.h \
../include/yarp/YARPMath.h \
../include/yarp/YARPMatrix.h \
../include/yarp/YARPRobotMath.h \
../include/yarp/YARPSimpleOperations.h \
../include/yarp/YARPSound.h \
../include/yarp/YARPSoundPortContent.h \
../include/yarp/YARPVectorPortContent.h \
../include/yarp/YARPVisMap.h \
../include/yarp/YARPMatrix.inl \
../include/yarp/YARPRobotMath.inl



libYARP_sig_compat_la_LDFLAGS = -rpath $(libdir)
