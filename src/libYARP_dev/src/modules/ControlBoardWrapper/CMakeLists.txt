
option(CBW2_USE_YARP_THRIFT "Use the extended status generted from .thrift file" OFf)
option(CBW2_USE_ROS_MSG "Create ROS topic" OFF)


if(CBW2_USE_YARP_THRIFT)
    add_definitions(-DYARP_MSG)

    set(thrifFile stateExt)
    ## Load yarp stuff
    find_package(YARP REQUIRED)     ## make cmake look for yarp
    list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})  ## add path of the yarp-cmake files full of wonderful macros
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})  ## include yarp header files

    include(YarpIDL)    ## wa want to use IDL so include their macros

    set(ALLOW_IDL_GENERATION ON CACHE BOOL "on")


    message(INFO " Using YARP message\n")
    #set(generated_libs_dir build/src)        # root folder where place generated files
    set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    yarp_idl_to_dir(${thrifFile}.thrift ${generated_libs_dir})     # run the generator engine

    #include files generated previously
    include(${thrifFile}_thrift.cmake)

    # create the sender
    set(yarp_msg_headers ${headers})
    set(yarp_msg_sources ${sources})
    message(STATUS " YARP headers is ${yarp_msg_headers}")
    message(STATUS " YARP sources is ${yarp_msg_sources}")

endif(CBW2_USE_YARP_THRIFT)

if(CBW2_USE_ROS_MSG)

        message(INFO " Using ROS message\n")
        add_definitions(-DROS_MSG)

    # prepare some stuff
        # name of the file
        set(rosMSG jointState)
        ## Load yarp stuff --> why it is required, I´m inside yarp after all. Is there a better way???
        find_package(YARP REQUIRED)     ## make cmake look for yarp
        list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})  ## add path of the yarp-cmake files full of wonderful macros
        include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})  ## include yarp header files

        include(YarpIDL)    ## we want to use IDL so include their macros
        set(ALLOW_IDL_GENERATION ON CACHE BOOL "on")  # force generation for now ... TBR


    #idl commands
        set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
        yarp_idl_to_dir(${rosMSG}.msg ${generated_libs_dir} ros_msg_sources ros_msg_headers include_dirs)
        include_directories(${include_dirs})
        include(${rosMSG}_msg.cmake)
        include_directories(${generated_libs_dir}/include ${YARP_INCLUDE_DIRS})

    #debug messages
        message(STATUS " ROS headers is ${ros_msg_headers}")
        message(STATUS " ROS sources is ${ros_msg_sources}")
        message(STATUS " ROS include_dirs is ${include_dirs}")
        message(STATUS " generated_libs_dir is ${generated_libs_dir}")


endif(CBW2_USE_ROS_MSG)

set(controlBoardWrapper_srcs    src/modules/ControlBoardWrapper/ControlBoardWrapper.cpp
                                src/modules/ControlBoardWrapper/RPCMessagesParser.cpp
                                src/modules/ControlBoardWrapper/StreamingMessagesParser.cpp
                                src/modules/ControlBoardWrapper/SubDevice.cpp)

set(controlBoardWrapper_hrds   src/modules/ControlBoardWrapper/ControlBoardWrapper.h
                                src/modules/ControlBoardWrapper/RPCMessagesParser.h
                                src/modules/ControlBoardWrapper/StreamingMessagesParser.h
                                src/modules/ControlBoardWrapper/SubDevice.h)


# add automatically generated files to source list
set(controlBoardWrapper_srcs ${controlBoardWrapper_srcs} ${yarp_msg_sources}  ${ros_msg_sources} )
set_property(GLOBAL APPEND PROPERTY controlBoardWrapper_files ${controlBoardWrapper_srcs})
