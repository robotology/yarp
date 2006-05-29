
# replace "${CMAKE_SOURCE_DIR}/.." with the path to your YARP binaries
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../build" CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

PROJECT(example)

ADD_EXECUTABLE(example main.cpp)
