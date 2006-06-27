# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

ADD_EXECUTABLE(simple_sender simple_sender.cpp)
ADD_EXECUTABLE(simple_receiver simple_receiver.cpp)
ADD_EXECUTABLE(internal_monologue internal_monologue.cpp)

