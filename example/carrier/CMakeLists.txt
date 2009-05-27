PROJECT (Carrier)

# replace "..." with the path to your YARP binaries if needed
#SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

ADD_EXECUTABLE(carrier_stub carrier_stub.cpp)

ADD_EXECUTABLE(carrier_human carrier_human.cpp)
