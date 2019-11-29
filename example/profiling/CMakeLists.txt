# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.12)
project(Profiling)

# replace "..." with the path to your YARP binaries if needed
set(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")
set(USE_PARALLEL_PORT FALSE CACHE BOOL "Do you want to use the parallel port?")

# profiling flags for linux
set(CMAKE_CXX_FLAGS_PROFILING "-pg" CACHE STRING "profiling flags")
# To use, set "CMAKE_BUILD_TYPE" in ccmake to "PROFILING"
# Then run program as normal, e.g. "./bottle_test"
# Then run with gprof prefix, e.g. "gprof ./bottle_test > result.txt"
# Look at output and think.

find_package(YARP REQUIRED)
link_libraries(${YARP_LIBRARIES})

find_package(ICUB)

if(USE_PARALLEL_PORT)
  find_package(PPEVENTDEBUGGER)
  include_directories(${PPEVENTDEBUGGER_INCLUDE_DIRS})
  add_definitions(-DUSE_PARALLEL_PORT)
endif()

add_executable(bottle_test bottle_test.cpp)
target_link_libraries(bottle_test ${YARP_LIBRARIES})
if(USE_PARALLEL_PORT)
  target_link_libraries(bottle_test ${PPEVENTDEBUGGER_LIBRARIES})
endif()

add_executable(port_latency port_latency.cpp)
target_link_libraries(port_latency ${YARP_LIBRARIES})
if(USE_PARALLEL_PORT)
  target_link_libraries(port_latency ${PPEVENTDEBUGGER_LIBRARIES})
endif()

add_executable(port_latency_st port_latency_st.cpp)
target_link_libraries(port_latency_st ${YARP_LIBRARIES})
if(USE_PARALLEL_PORT)
  target_link_libraries(port_latency_st ${PPEVENTDEBUGGER_LIBRARIES})
endif()

add_executable(thread_latency thread_latency.cpp)
target_link_libraries(thread_latency ${YARP_LIBRARIES})
if(USE_PARALLEL_PORT)
  target_link_libraries(thread_latency ${PPEVENTDEBUGGER_LIBRARIES})
endif()

add_executable(timers timers.cpp)
target_link_libraries(timers ${YARP_LIBRARIES})
if(USE_PARALLEL_PORT)
  target_link_libraries(timers ${PPEVENTDEBUGGER_LIBRARIES})
endif()

add_executable(rateThreadTiming rateThreadTiming.cpp)
target_link_libraries(rateThreadTiming ${YARP_LIBRARIES})
if(USE_PARALLEL_PORT)
  target_link_libraries(rateThreadTiming ${PPEVENTDEBUGGER_LIBRARIES})
endif()
