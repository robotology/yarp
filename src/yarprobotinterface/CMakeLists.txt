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

set(robotInterface_IDLS
    robotInterface.thrift)

include(YarpIDL)
yarp_idl_to_dir(${robotInterface_IDLS}
                "${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code"
                robotInterface_IDLS_SRCS
                robotInterface_IDLS_HDRS
                robotInterface_IDLS_INCLUDE_DIRS)
include_directories("${robotInterface_IDLS_INCLUDE_DIRS}")

add_executable(robotInterface ${robotInterface_SRCS}
                              ${robotInterface_HDRS}
                              ${robotInterface_IDLS_SRCS}
                              ${robotInterface_IDLS_HDRS})

target_link_libraries(robotInterface
                      YARP::YARP_OS
                      YARP::YARP_dev
                      YARP::YARP_init
                      ${TinyXML_LIBRARIES})

install(TARGETS robotInterface
        DESTINATION bin
        COMPONENT Runtime)
