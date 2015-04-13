

# include(YarpIDL)    ## we want to use IDL so include their macros

## Extended yarp port always on by default, backward compatibility is provided by a run-time check

# name of the file
set(thrifFile stateExt)

#idl commands
set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}/yarp")

# run the generator engine
#yarp_idl_to_dir(${thrifFile}.thrift ${generated_libs_dir} yarp_msg_sources yarp_msg_headers yarp_msg_include_dirs)

#include files generated previously
include(${generated_libs_dir}/${thrifFile}_thrift.cmake)

foreach(file ${sources})
    set(yarp_msg_sources ${yarp_msg_sources} ${generated_libs_dir}/${file})
endforeach()

foreach(file ${headers})
    set(yarp_msg_headers ${yarp_msg_headers} ${generated_libs_dir}/${file})
endforeach()

set(yarp_msg_include_dirs "${generated_libs_dir}/include")

include_directories(${yarp_msg_include_dirs} )

# create the sender
#message(STATUS ">> YARP MSG sources is ${yarp_msg_sources}")
#message(STATUS ">> YARP MSG headers is ${yarp_msg_headers}")
#message(STATUS ">> YARP MSG yarp_msg_include_dirs is ${yarp_msg_include_dirs}")


    # name of the file
    set(rosMSG sensorMsgs_JointState; sensorMsgs_Imu; geometryMsgs_WrenchedStamped)

    #idl commands
    set(ROS_msgs_dir "${CMAKE_CURRENT_SOURCE_DIR}/ros")

    # messages are generated via the command yarpidl_rosmsg <message_name> like 'yarpidl_rosmsg sensor_msg/JointState'

    # Collect all header file from all messages into a single variable
    foreach(cmake_file ${rosMSG})
        include("${ROS_msgs_dir}/${cmake_file}.cmake")

        foreach(file ${sources})
            set(ros_msg_sources ${ros_msg_sources} ${ROS_msgs_dir}/${file})
        endforeach()

        foreach(file ${headers})
            set(ros_msg_headers ${ros_msg_headers} ${ROS_msgs_dir}/${file})
        endforeach()

        # Remove duplicated reference to header files
        list(LENGTH ros_msg_headers len)
        if(len  GREATER 0)
            list(REMOVE_DUPLICATES ros_msg_headers )
        endif()

        list(LENGTH ros_msg_sources len)
        if(len  GREATER 0)
            list(REMOVE_DUPLICATES ros_msg_sources )
        endif()

    endforeach()
    set(ros_msg_include_dirs "${ROS_msgs_dir}/include")

    include_directories(${ROS_msgs_include_dirs} )

    #debug messages
    #message(STATUS ">> ROS_msgs_dir is ${ROS_msgs_dir}")
    #message(STATUS ">> ROS headers is ${ros_msg_headers}")
    #message(STATUS ">> ROS sources is ${ros_msg_sources}")
    #message(STATUS ">> ROS include_dirs is ${ros_msg_include_dirs}")


# add automatically generated files to source list
set(cbw2_msgs_srcs        ${yarp_msg_sources} ${ros_msg_sources}   PARENT_SCOPE)
set(cbw2_msgs_hrds        ${yarp_msg_headers} ${ros_msg_headers}   PARENT_SCOPE)
set(cbw2_msgs_incl_dirs   ${yarp_msg_include_dirs} ${ros_msg_include_dirs} PARENT_SCOPE)
