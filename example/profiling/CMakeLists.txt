# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

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

find_package(YARP COMPONENTS os REQUIRED)

if(USE_PARALLEL_PORT)
  find_package(PPEVENTDEBUGGER)
endif()

add_executable(bottle_test)
target_sources(bottle_test PRIVATE bottle_test.cpp)
target_link_libraries(bottle_test PRIVATE YARP::YARP_os YARP::YARP_init)
if(USE_PARALLEL_PORT)
  target_include_directories(bottle_test PRIVATE ${PPEVENTDEBUGGER_INCLUDE_DIRS})
  target_link_libraries(bottle_test PRIVATE ${PPEVENTDEBUGGER_LIBRARIES})
  target_compile_definitions(bottle_test PRIVATE USE_PARALLEL_PORT)
endif()

add_executable(port_latency)
target_sources(port_latency PRIVATE port_latency.cpp)
target_link_libraries(port_latency PRIVATE YARP::YARP_os YARP::YARP_init)
if(USE_PARALLEL_PORT)
  target_include_directories(port_latency PRIVATE ${PPEVENTDEBUGGER_INCLUDE_DIRS})
  target_link_libraries(port_latency PRIVATE ${PPEVENTDEBUGGER_LIBRARIES})
  target_compile_definitions(port_latency PRIVATE USE_PARALLEL_PORT)
endif()

add_executable(port_latency_st)
target_sources(port_latency_st PRIVATE port_latency_st.cpp)
target_link_libraries(port_latency_st PRIVATE YARP::YARP_os YARP::YARP_init)
if(USE_PARALLEL_PORT)
  target_include_directories(port_latency_st PRIVATE ${PPEVENTDEBUGGER_INCLUDE_DIRS})
  target_link_libraries(port_latency_st PRIVATE ${PPEVENTDEBUGGER_LIBRARIES})
  target_compile_definitions(port_latency_st PRIVATE USE_PARALLEL_PORT)
endif()

add_executable(thread_latency)
target_sources(thread_latency PRIVATE thread_latency.cpp)
target_link_libraries(thread_latency PRIVATE YARP::YARP_os YARP::YARP_init)
if(USE_PARALLEL_PORT)
  target_include_directories(thread_latency PRIVATE ${PPEVENTDEBUGGER_INCLUDE_DIRS})
  target_link_libraries(thread_latency PRIVATE ${PPEVENTDEBUGGER_LIBRARIES})
  target_compile_definitions(thread_latency PRIVATE USE_PARALLEL_PORT)
endif()

add_executable(timers)
target_sources(timers PRIVATE timers.cpp)
target_link_libraries(timers PRIVATE YARP::YARP_os YARP::YARP_init)
if(USE_PARALLEL_PORT)
  target_include_directories(timers PRIVATE ${PPEVENTDEBUGGER_INCLUDE_DIRS})
  target_link_libraries(timers PRIVATE ${PPEVENTDEBUGGER_LIBRARIES})
  target_compile_definitions(timers PRIVATE USE_PARALLEL_PORT)
endif()

add_executable(rateThreadTiming)
target_sources(rateThreadTiming PRIVATE rateThreadTiming.cpp)
target_link_libraries(rateThreadTiming PRIVATE YARP::YARP_os YARP::YARP_init)
if(USE_PARALLEL_PORT)
  target_include_directories(rateThreadTiming PRIVATE ${PPEVENTDEBUGGER_INCLUDE_DIRS})
  target_link_libraries(rateThreadTiming PRIVATE ${PPEVENTDEBUGGER_LIBRARIES})
  target_compile_definitions(rateThreadTiming PRIVATE USE_PARALLEL_PORT)
endif()
