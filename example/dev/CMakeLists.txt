# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

ADD_EXECUTABLE(fake_grabber fake_grabber.cpp FakeFrameGrabber.h)
ADD_EXECUTABLE(fake_grabber_net fake_grabber_net.cpp FakeFrameGrabber.h)
