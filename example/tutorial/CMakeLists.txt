SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

# Start a project.
PROJECT(run_fakebot)

SET(ENABLE_fakebot TRUE CACHE BOOLEAN "we want to use fakebot")

# Search for source code.
FILE(GLOB_RECURSE folder_source *.cpp *.cc *.c)
FILE(GLOB_RECURSE folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/fakebot)

# Set up our main executable.
IF (folder_source)
  ADD_EXECUTABLE(run_fakebot ${folder_source} ${folder_header})
ELSE (folder_source)
  MESSAGE(FATAL_ERROR "No source code files found. Please add something")
ENDIF (folder_source)
