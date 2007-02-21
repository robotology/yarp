SET(KEYWORD "run_fakebot")

SET(ENABLE_fakebot TRUE CACHE BOOLEAN "we want to use fakebot")

SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

# Start a project.
PROJECT(${KEYWORD})

# Search for source code.
FILE(GLOB folder_source *.cpp *.cc *.c)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

# Check if there are any plugin device subdirectories.
# Device subdirectories contain a config.cmake file.
FILE(GLOB_RECURSE device_config config.cmake)
IF (device_config)
  YarpDevice(${device_config})
ENDIF (device_config)

# Automatically add include directories if needed.
FOREACH(header_file ${folder_header})
  GET_FILENAME_COMPONENT(p ${header_file} PATH)
  INCLUDE_DIRECTORIES(${p})
ENDFOREACH(header_file ${folder_header})

# Set up our main executable.
IF (folder_source)
  ADD_EXECUTABLE(${KEYWORD} ${folder_source} ${folder_header})
ELSE (folder_source)
  MESSAGE(FATAL_ERROR "No source code files found. Please add something")
ENDIF (folder_source)
