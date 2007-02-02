
PROJECT(gtkdemo)

SET(YARP_DIR "$ENV{YARP_ROOT}" CACHE LOCATION "where is yarp?")
SET(CMAKE_MODULE_PATH "$ENV{YARP_ROOT}/conf")

INCLUDE(UsePkgConfig)
FIND_PACKAGE(YARP REQUIRED)
FIND_PACKAGE(GtkPlus REQUIRED)

ADD_DEFINITIONS(${GTKPLUS_C_FLAGS})
LINK_LIBRARIES(${GTKPLUS_LINK_FLAGS})
INCLUDE_DIRECTORIES(${GTKPLUS_INCLUDE_DIR})

FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h include/*.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

ADD_EXECUTABLE(gtkdemo ${folder_source} ${folder_header})

