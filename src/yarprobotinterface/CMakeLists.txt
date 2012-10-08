project(robot-interface)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(YARP REQUIRED)
find_package(TinyXML REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})

set(robotInterface_SRCS
    main.cpp
    RobotInterface.cpp
    XMLReader.cpp
)

set(robotInterface_HDRS
    RobotInterface.h
    XMLReader.h
)

add_executable(robot-interface ${robotInterface_SRCS})

target_link_libraries(robot-interface
                      YARP_OS
                      ${TinyXML_LIBRARIES})
