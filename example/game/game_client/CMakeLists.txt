SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../../.." CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

PROJECT(game_client)

INCLUDE_DIRECTORIES($(CMAKE_SOURCE_DIR))

IF (NOT WIN32)
   LINK_LIBRARIES(ncurses)
ENDIF (NOT WIN32)

ADD_EXECUTABLE(game_client main.cpp keyboard.cpp simio.cpp)

