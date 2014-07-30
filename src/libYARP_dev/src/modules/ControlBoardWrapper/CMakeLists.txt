set(controlBoardWrapper_srcs    src/modules/ControlBoardWrapper/ControlBoardWrapper.cpp
                                src/modules/ControlBoardWrapper/RPCMessagesParser.cpp
                                src/modules/ControlBoardWrapper/StreamingMessagesParser.cpp
                                src/modules/ControlBoardWrapper/SubDevice.cpp)

set(controlBoardWrapper_hrds   src/modules/ControlBoardWrapper/ControlBoardWrapper.h
                                src/modules/ControlBoardWrapper/RPCMessagesParser.h
                                src/modules/ControlBoardWrapper/StreamingMessagesParser.h
                                src/modules/ControlBoardWrapper/SubDevice.h)

set(use_thrift false)
set(use_ros true)

if(use_thrift)
    add_definitions(-DYARP_MSG)

    set(thrifFile stateExt)
    ## Load yarp stuff
    find_package(YARP REQUIRED)     ## make cmake look for yarp
    list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})  ## add path of the yarp-cmake files full of wonderful macros
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})  ## include yarp header files

    include(YarpIDL)    ## wa want to use IDL so include their macros

    set(ALLOW_IDL_GENERATION ON CACHE BOOL "on")


    message(INFO " way is true\n")
    #set(generated_libs_dir build/src)        # root folder where place generated files
    set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    yarp_idl_to_dir(${thrifFile}.thrift ${generated_libs_dir})     # run the generator engine
#    yarp_idl_to_dir(${thrifFile}2.msg ${generated_libs_dir})     # run the generator engine

    #include files generated previously
    include(${thrifFile}_thrift.cmake)

    # create the sender
    message(STATUS " YARP headers is ${headers}")
    message(STATUS " YARP sources is ${sources}")

endif(use_thrift)

if(use_ros)
    add_definitions(-DROS_MSG)

    set(rosMSG jointState)
    ## Load yarp stuff
    find_package(YARP REQUIRED)     ## make cmake look for yarp
    list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH})  ## add path of the yarp-cmake files full of wonderful macros
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})  ## include yarp header files

    include(YarpIDL)    ## wa want to use IDL so include their macros

    set(ALLOW_IDL_GENERATION ON CACHE BOOL "on")


    message(INFO " way is true\n")
    #set(generated_libs_dir build/src)        # root folder where place generated files
    set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}")
    yarp_idl_to_dir(${rosMSG}.msg ${generated_libs_dir})     # run the generator engine

    string(TOLOWER ${rosMSG} rosLowerName)
    message(STATUS "generated_libs_dir is ${generated_libs_dir} ||")
    message(STATUS "include folder should be ${CMAKE_CURRENT_BINARY_DIR}/${rosLowerName} ||")


    #include files generated previously
    include(${rosMSG}_msg.cmake)

    # create the sender
    message(STATUS " ROS headers is ${headers}")
    message(STATUS " ROS sources is ${sources}")

    include_directories(${CMAKE_CURRENT_BINARY_DIR}/${rosLowerName})

endif(use_ros)

#foreach(file in ${headers})
#    set(header_files ${header_files} ${CMAKE_CURRENT_SOURCE_DIR}/${generated_libs_dir}/${file})
#    message(INFO " header_files is ${header_files}")
#endforeach(file)

#message(INFO " header_files is ${header_files}")

# add automatically generated files to source list
set(controlBoardWrapper_srcs ${controlBoardWrapper_srcs} ${sources} )
set_property(GLOBAL APPEND PROPERTY controlBoardWrapper_files ${controlBoardWrapper_srcs})
