PROJECT(PortPower)

# replace "..." with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

FILE(GLOB_RECURSE examples *.cpp)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

FOREACH(example ${examples})
	# get name without extension
	GET_FILENAME_COMPONENT(example_we "${example}" NAME_WE)
	ADD_EXECUTABLE(${example_we} ${example})
ENDFOREACH(example ${examples})
