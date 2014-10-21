
option(CBW2_USE_YARP_THRIFT "Use the extended status generated from .thrift file" OFf)
option(CBW2_USE_ROS_MSG "Create ROS topic" OFF)

include(YarpIDL)    ## we want to use IDL so include their macros

if(CBW2_USE_YARP_THRIFT)
    message(INFO " Using YARP message\n")
    add_definitions(-DYARP_MSG)

    # name of the file
    set(thrifFile stateExt)

    #idl commands
    set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

    # run the generator engine
    yarp_idl_to_dir(${thrifFile}.thrift ${generated_libs_dir} yarp_msg_sources yarp_msg_headers yarp_msg_include_dirs)

    #include files generated previously
    include(${generated_libs_dir}/${thrifFile}_thrift.cmake)
#    include_directories(${yarp_msg_include_dirs} )

    # create the sender
    message(STATUS ">> YARP MSG sources is ${yarp_msg_sources}")
    message(STATUS ">> YARP MSG headers is ${yarp_msg_headers}")
    message(STATUS ">> YARP MSG yarp_msg_include_dirs is ${yarp_msg_include_dirs}")


endif(CBW2_USE_YARP_THRIFT)

if(CBW2_USE_ROS_MSG)
    message(INFO " Using ROS message\n")
    add_definitions(-DROS_MSG)

    # name of the file
    set(rosMSG jointState)

    #idl commands
    set(generated_libs_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

    # run the generator engine
    yarp_idl_to_dir(${rosMSG}.msg ${generated_libs_dir} ros_msg_sources ros_msg_headers ros_msg_include_dirs)

    #include files generated previously
    include(${generated_libs_dir}/${rosMSG}_msg.cmake)
    #include_directories( ${generated_libs_dir}/include ${YARP_INCLUDE_DIRS})

    #debug messages
    message(STATUS ">> ROS headers is ${ros_msg_headers}")
    message(STATUS ">> ROS sources is ${ros_msg_sources}")
    message(STATUS ">> ROS include_dirs is ${ros_msg_include_dirs}")
    message(STATUS ">> generated_libs_dir is ${generated_libs_dir}")

endif(CBW2_USE_ROS_MSG)




# add automatically generated files to source list
set(cbw2_msgs_srcs        ${yarp_msg_sources} ${ros_msg_sources}   PARENT_SCOPE)
set(cbw2_msgs_hrds        ${yarp_msg_headers} ${ros_msg_headers}   PARENT_SCOPE)
set(cbw2_msgs_incl_dirs   ${yarp_msg_include_dirs} ${ros_msg_include_dirs} PARENT_SCOPE)
