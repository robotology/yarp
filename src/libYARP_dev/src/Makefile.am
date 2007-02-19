
INCLUDES = -I$(srcdir)/../include -I$(srcdir)/../../libYARP_OS/include -I$(srcdir)/../../libYARP_sig/include -I$(srcdir)/../../libYARP_dev/include

lib_LTLIBRARIES = libYARP_dev.la

# automake_expand libYARP_dev_la_SOURCES_direct *.cpp
libYARP_dev_la_SOURCES_direct = ControlBoardPid.cpp ControlCalibrationImpl.cpp DeviceDriver.cpp DeviceGroup.cpp DevicePipe.cpp Drivers.cpp PolyDriver.cpp RemoteControlBoard.cpp RemoteFrameGrabber.cpp ServerFrameGrabber.cpp TestFrameGrabber.cpp 

libYARP_dev_la_SOURCES = $(libYARP_dev_la_SOURCES_direct) ../generated_src/PopulateDrivers.cpp

libYARP_dev_la_LDFLAGS = -rpath $(libdir)
