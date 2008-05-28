PROJECT (Profiing)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

ADD_EXECUTABLE(bottle_test bottle_test.cpp)
ADD_EXECUTABLE(port_latency  port_latency.cpp)
ADD_EXECUTABLE(port_latency_st  port_latency_st.cpp)
