PROJECT (yayarpview)

# replace with the path to your YARP binaries if needed
SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../.." CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)

FIND_PACKAGE(X11)

INCLUDE_DIRECTORIES(${X11_INCLUDE_DIR})
LINK_LIBRARIES(${X11_LIBRARIES})

# LINK_LIBRARIES("-ljpeg -lpng -lpthread -lm")
# ADD_DEFINITIONS("-Dcimg_use_png -Dcimg_use_jpeg")
LINK_LIBRARIES("-lpthread -lm")

FILE(GLOB HEADERS *.h)
FILE(GLOB CODE *.cpp)

ADD_EXECUTABLE(yayarpview ${HEADERS} ${CODE})
