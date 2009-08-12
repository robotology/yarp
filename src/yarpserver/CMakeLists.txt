CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

PROJECT(yarpdb)

# Useful for building executables that can be distributed without problems -
# ignore otherwise.
SET (STATLIB "${CMAKE_BINARY_DIR}/static_libs")
IF (EXISTS ${STATLIB})
    MESSAGE(STATUS "static_libs directory present: ${STATLIB}")
    LINK_DIRECTORIES(${STATLIB})
    ADD_DEFINITIONS(-static-libgcc)
    SET(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc")
    FILE(GLOB statlibs ${STATLIB}/*.a)
    LINK_LIBRARIES(${statlibs})
ENDIF (EXISTS ${STATLIB})


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/sqlite)

ADD_LIBRARY(triplesql src/TripleSourceCreator.cpp src/TripleSourceCreator.h src/Triple.h src/TripleSource.h src/SqliteTripleSource.h sqlite/sqlite3.c sqlite/sqlite3.h)

IF (NOT WIN32)
  TARGET_LINK_LIBRARIES(triplesql pthread dl)
ENDIF (NOT WIN32)

ADD_EXECUTABLE(server_peek src/server_peek.cpp)
TARGET_LINK_LIBRARIES(server_peek triplesql)

# Find YARP.  Point the YARP_DIR environment variable at your build.
FIND_PACKAGE(YARP)
ADD_EXECUTABLE(yarpserver3 src/yarpserver3.cpp src/NameServiceOnTriples.cpp src/NameServiceOnTriples.h src/NameServerConnectionHandler.h src/NameServerManager.h src/BootstrapServer.h src/BootstrapServer.cpp src/Allocator.h src/AllocatorOnTriples.cpp src/AllocatorOnTriples.h)
TARGET_LINK_LIBRARIES(yarpserver3 triplesql)

ADD_EXECUTABLE(server_test src/server_test.cpp)
