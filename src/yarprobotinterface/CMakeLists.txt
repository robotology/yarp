project(robot-interface)
cmake_minimum_required(VERSION 2.6.4)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})


#### Find YARP, require that YARP version is at least YARP_REQ_VERSION
set(YARP_REQ_VERSION_MAJOR "2")
set(YARP_REQ_VERSION_MINOR "3")
set(YARP_REQ_VERSION_PATCH "20")
set(YARP_REQ_VERSION_TWEAK "")
set(YARP_REQ_VERSION ${YARP_REQ_VERSION_MAJOR}.${YARP_REQ_VERSION_MINOR}.${YARP_REQ_VERSION_PATCH})
#set(YARP_REQ_VERSION ${YARP_REQ_VERSION_MAJOR}.${YARP_REQ_VERSION_MINOR}.${YARP_REQ_VERSION_PATCH}.${YARP_REQ_VERSION_TWEAK})
find_package(YARP REQUIRED)

if (YARP_VERSION VERSION_LESS YARP_REQ_VERSION)
    message(FATAL_ERROR "YARP version is not compatible, please update YARP")
else()
    message(STATUS "found compatible YARP version (${YARP_VERSION})")
endif()

find_package(ICUB REQUIRED)
find_package(TinyXML 2.6 REQUIRED)

include_directories(${YARP_INCLUDE_DIRS})
include_directories(${iCubDev_INCLUDE_DIRS})
include_directories(${TinyXML_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/libraries/icubmod/)

set(robotInterface_SRCS
    main.cpp
    Action.cpp
    CalibratorThread.cpp
    Device.cpp
    Module.cpp
    Param.cpp
    Robot.cpp
    Types.cpp
    XMLReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/libraries/icubmod/debugStream/Debug.cpp
)

set(robotInterface_HDRS
    Action.h
    CalibratorThread.h
    Device.h
    Module.h
    Param.h
    Robot.h
    Types.h
    XMLReader.h
)

#add_definitions("-Wall -Wextra --ansi --pedantic")
#add_definitions("-Wno-long-long -Wno-variadic-macros -Wshadow")
add_definitions("-DDEBUG")

add_executable(robot-interface ${robotInterface_SRCS})

target_link_libraries(robot-interface
                      YARP_OS
                      YARP_init
                      icubmod
                    #  debugStream
                      ${TinyXML_LIBRARIES})
