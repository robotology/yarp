PROJECT (Profiing)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

FIND_PACKAGE(ICUB)
FIND_PACKAGE(PPEVENTDEBUGGER)

LINK_LIBRARIES(
  ${PPEVENTDEBUGGER_LIBRARIES}
)

INCLUDE_DIRECTORIES(
 ${PPEVENTDEBUGGER_INCLUDE_DIRS}
)

ADD_EXECUTABLE(bottle_test bottle_test.cpp)
ADD_EXECUTABLE(port_latency  port_latency.cpp)
ADD_EXECUTABLE(port_latency_st  port_latency_st.cpp)
ADD_EXECUTABLE(thread_latency  thread_latency.cpp)
ADD_EXECUTABLE(rateThreadTiming rateThreadTiming.cpp)
