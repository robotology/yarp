SET(YARP_DIR "${CMAKE_SOURCE_DIR}/../../.." CACHE LOCATION "where is yarp?")
FIND_PACKAGE(YARP)

PROJECT(game_server)

INCLUDE_DIRECTORIES($(CMAKE_SOURCE_DIR))

ADD_EXECUTABLE(game_server Game.cpp  Login.cpp main.cpp  Player.cpp  Thing.cpp  Things.cpp)
