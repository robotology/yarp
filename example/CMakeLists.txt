
# replace ".." with the path to your YARP binaries
SET(YARP_DIR ..)

FIND_PACKAGE(YARP)

PROJECT(example)

ADD_EXECUTABLE(example main.cpp)
