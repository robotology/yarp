project(robot-interface)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(YARP REQUIRED)
find_package(ICUB REQUIRED)
find_package(TinyXML REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})
include_directories(${iCubDev_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/libraries/icubmod/DebugStream)

set(robotInterface_SRCS
    main.cpp
    Module.cpp
    RobotInterface.cpp
    XMLReader.cpp
    drivers/ControlBoardWrapper.cpp
)

set(robotInterface_HDRS
    Module.h
    RobotInterface.h
    XMLReader.h
    drivers/ControlBoardWrapper.h
    drivers/debugging.h
)

#add_definitions("-Wall -Wextra --ansi --pedantic")
#add_definitions("-Wno-long-long -Wno-variadic-macros -Wshadow")

add_executable(robot-interface ${robotInterface_SRCS})

target_link_libraries(robot-interface
                      YARP_OS
                      YARP_init
                      icubmod
                      DebugStream
                      ${TinyXML_LIBRARIES})
