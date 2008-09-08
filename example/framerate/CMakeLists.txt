# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)
FIND_PACKAGE(Ace)

ADD_EXECUTABLE(framerate main.cpp)
INSTALL_TARGETS(/bin framerate)

