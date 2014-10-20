
option(CBW2_USE_YARP_THRIFT "Use the extended status generated from .thrift file" OFf)
option(CBW2_USE_ROS_MSG "Create ROS topic" OFF)


if(CBW2_USE_YARP_THRIFT)
    add_definitions(-DYARP_MSG)

    set(thrifFile stateExt)
    ## Load yarp stuff
#    find_package(YARP REQUIRED)     ## make cmake look for yarp
#    list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})  ## add path of the yarp-cmake files full of wonderful macros
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})  ## include yarp header files

    include(YarpIDL)    ## we want to use IDL so include their macros

    set(ALLOW_IDL_GENERATION ON CACHE BOOL "on")


    message(INFO " Using YARP message\n")
    set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
    yarp_idl_to_dir(${thrifFile}.thrift ${generated_libs_dir} yarp_msg_sources yarp_msg_headers yarp_msg_include_dirs)     # run the generator engine

    #include files generated previously
    include(${generated_libs_dir}/${thrifFile}_thrift.cmake)
    include_directories( ${generated_libs_dir}/include ${yarp_msg_include_dirs} ${YARP_INCLUDE_DIRS})

    # create the sender
    message(STATUS ">> YARP MSG headers is ${yarp_msg_headers}")
    message(STATUS ">> YARP MSG sources is ${yarp_msg_sources}")
    message(STATUS ">> YARP MSG yarp_msg_include_dirs is ${yarp_msg_include_dirs}")

    set_property(GLOBAL APPEND PROPERTY controlBoardWrapper_files ${controlBoardWrapper_srcs})
    set_property(GLOBAL APPEND PROPERTY controlBoardWrapper_include_paths "${generated_libs_dir}/_yarp_idl_" )

endif(CBW2_USE_YARP_THRIFT)

if(CBW2_USE_ROS_MSG)

        message(INFO " Using ROS message\n")
        add_definitions(-DROS_MSG)

    # prepare some stuff
        # name of the file
        set(rosMSG jointState)

        include(YarpIDL)    ## we want to use IDL so include their macros
        set(ALLOW_IDL_GENERATION ON CACHE BOOL "on")  # force generation for now ... TBR

    #idl commands
        set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
        yarp_idl_to_dir(${rosMSG}.msg ${generated_libs_dir} ros_msg_sources ros_msg_headers ros_msg_include_dirs)
        include_directories(${include_dirs})
        include(${generated_libs_dir}/${rosMSG}_msg.cmake)
        include_directories( ${generated_libs_dir}/include ${YARP_INCLUDE_DIRS})

    #debug messages
        message(STATUS ">> ROS headers is ${ros_msg_headers}")
        message(STATUS ">> ROS sources is ${ros_msg_sources}")
        message(STATUS ">> ROS include_dirs is ${ros_msg_include_dirs}")
        message(STATUS ">> generated_libs_dir is ${generated_libs_dir}")


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
set_property(GLOBAL APPEND PROPERTY controlBoardWrapper_include_paths ${yarp_msg_include_dirs} ${ros_msg_include_dirs} )
