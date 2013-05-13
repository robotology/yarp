# Copyright: (C) 2009 RobotCub Consortium
# Authors: Lorenzo Natale, Paul Fitzpatrick, Giorgio Metta
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
cmake_minimum_required(VERSION 2.8.7)

PROJECT (Profiling)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")
SET(USE_PARALLEL_PORT FALSE CACHE BOOL "Do you want to use the parallel port?")

# profiling flags for linux
SET(CMAKE_CXX_FLAGS_PROFILING "-pg" CACHE STRING "profiling flags")
# To use, set "CMAKE_BUILD_TYPE" in ccmake to "PROFILING"
# Then run program as normal, e.g. "./bottle_test"
# Then run with gprof prefix, e.g. "gprof ./bottle_test > result.txt"
# Look at output and think.

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
LINK_LIBRARIES(${YARP_LIBRARIES})

FIND_PACKAGE(ICUB)

IF (USE_PARALLEL_PORT)
  FIND_PACKAGE(PPEVENTDEBUGGER)
  LINK_LIBRARIES(
    ${PPEVENTDEBUGGER_LIBRARIES}
    )
  INCLUDE_DIRECTORIES(
    ${PPEVENTDEBUGGER_INCLUDE_DIRS}
    )
  ADD_DEFINITIONS(-DUSE_PARALLEL_PORT)  
ENDIF(USE_PARALLEL_PORT)

ADD_EXECUTABLE(bottle_test bottle_test.cpp)
ADD_EXECUTABLE(port_latency  port_latency.cpp)
ADD_EXECUTABLE(port_latency_st  port_latency_st.cpp)
ADD_EXECUTABLE(thread_latency  thread_latency.cpp)
ADD_EXECUTABLE(timers  timers.cpp)
ADD_EXECUTABLE(rateThreadTiming rateThreadTiming.cpp)
