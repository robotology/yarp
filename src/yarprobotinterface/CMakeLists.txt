project(robot-interface)

#set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})


#find_package(TinyXML 2.6 QUIET)

#if(TinyXML_FOUND)
#    option(USE_SYSTEM_TINYXML ON "")
#endif()
#if(NOT USE_SYSTEM_TINYXML)
    add_subdirectory(extern)
#endif()


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

add_executable(robotInterface ${robotInterface_SRCS})

target_link_libraries(robotInterface
                      YARP::YARP_OS
                      YARP::YARP_dev
                      YARP::YARP_init
                      icubmod
                    #  debugStream
                      ${TinyXML_LIBRARIES})

install(TARGETS robotInterface
        DESTINATION bin
        COMPONENT Runtime)
