

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
    set(rosMSG jointState)

    #idl commands
    set(generated_libs_dir "${CMAKE_CURRENT_SOURCE_DIR}/ros")

    # run the generator engine
    #yarp_idl_to_dir(${rosMSG}.msg ${generated_libs_dir} ros_msg_sources ros_msg_headers ros_msg_include_dirs)

    #include files generated previously
    include(${generated_libs_dir}/${rosMSG}_msg.cmake)

    foreach(file ${sources})
        set(ros_msg_sources ${ros_msg_sources} ${generated_libs_dir}/${file})
    endforeach()

    foreach(file ${headers})
        set(ros_msg_headers ${ros_msg_headers} ${generated_libs_dir}/${file})
    endforeach()

    set(ros_msg_include_dirs "${generated_libs_dir}/include")

    include_directories(${yarp_msg_include_dirs} )

    #debug messages
    #message(STATUS ">> ROS headers is ${ros_msg_headers}")
    #message(STATUS ">> ROS sources is ${ros_msg_sources}")
    #message(STATUS ">> ROS include_dirs is ${ros_msg_include_dirs}")
    #message(STATUS ">> generated_libs_dir is ${generated_libs_dir}")


# add automatically generated files to source list
set(cbw2_msgs_srcs        ${yarp_msg_sources} ${ros_msg_sources}   PARENT_SCOPE)
set(cbw2_msgs_hrds        ${yarp_msg_headers} ${ros_msg_headers}   PARENT_SCOPE)
set(cbw2_msgs_incl_dirs   ${yarp_msg_include_dirs} ${ros_msg_include_dirs} PARENT_SCOPE)
