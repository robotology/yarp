project(yarprobotinterface)

get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)


include_directories(${YARP_OS_INCLUDE_DIRS}
                    ${YARP_dev_INCLUDE_DIRS}
                    ${TinyXML_INCLUDE_DIRS})

set(yarprobotinterface_SRCS
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

set(yarprobotinterface_HDRS
    Action.h
    CalibratorThread.h
    Device.h
    Module.h
    Param.h
    Robot.h
    Types.h
    XMLReader.h
)

set(yarprobotinterface_IDLS
    yarprobotinterface.thrift)

include(YarpIDL)
yarp_idl_to_dir(${yarprobotinterface_IDLS}
                "${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code"
                yarprobotinterface_IDLS_SRCS
                yarprobotinterface_IDLS_HDRS
                yarprobotinterface_IDLS_INCLUDE_DIRS)
include_directories("${yarprobotinterface_IDLS_INCLUDE_DIRS}")

add_executable(yarprobotinterface ${yarprobotinterface_SRCS}
                              ${yarprobotinterface_HDRS}
                              ${yarprobotinterface_IDLS_SRCS}
                              ${yarprobotinterface_IDLS_HDRS})

target_link_libraries(yarprobotinterface
                      YARP_OS
                      YARP_dev
                      YARP_init
                      ${TinyXML_LIBRARIES})

install(TARGETS yarprobotinterface
        COMPONENT utilities
        DESTINATION ${CMAKE_INSTALL_BINDIR})
