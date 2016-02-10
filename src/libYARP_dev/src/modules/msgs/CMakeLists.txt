include(YarpIDL)

yarp_idl_to_dir("${CMAKE_CURRENT_SOURCE_DIR}/stateExt.thrift"
                "${CMAKE_CURRENT_SOURCE_DIR}/yarp"
                yarp_msg_sources
                yarp_msg_headers
                yarp_msg_include_dirs)
include("${CMAKE_CURRENT_SOURCE_DIR}/yarp/stateExt_thrift.cmake")

yarp_idl_to_dir("${CMAKE_CURRENT_SOURCE_DIR}/ros/msg_file/sensor_msgs/JointState.msg"
                "${CMAKE_CURRENT_SOURCE_DIR}/ros"
                ros_msg_sources
                ros_msg_headers
                ros_msg_include_dirs)
include(${CMAKE_CURRENT_SOURCE_DIR}/ros/sensor_msgs_JointState_msg.cmake)

yarp_idl_to_dir("${CMAKE_CURRENT_SOURCE_DIR}/ros/msg_file/sensor_msgs/Imu.msg"
                "${CMAKE_CURRENT_SOURCE_DIR}/ros"
                ros_msg_sources
                ros_msg_headers
                ros_msg_include_dirs)
include(${CMAKE_CURRENT_SOURCE_DIR}/ros/sensor_msgs_Imu_msg.cmake)

yarp_idl_to_dir("${CMAKE_CURRENT_SOURCE_DIR}/ros/msg_file/sensor_msgs/LaserScan.msg"
                "${CMAKE_CURRENT_SOURCE_DIR}/ros"
                ros_msg_sources
                ros_msg_headers
                ros_msg_include_dirs)
include(${CMAKE_CURRENT_SOURCE_DIR}/ros/sensor_msgs_LaserScan_msg.cmake)

yarp_idl_to_dir("${CMAKE_CURRENT_SOURCE_DIR}/ros/msg_file/geometry_msgs/WrenchStamped.msg"
                "${CMAKE_CURRENT_SOURCE_DIR}/ros"
                ros_msg_sources
                ros_msg_headers
                ros_msg_include_dirs)
include(${CMAKE_CURRENT_SOURCE_DIR}/ros/geometry_msgs_WrenchStamped_msg.cmake)

# # add automatically generated files to source list
set(cbw2_msgs_srcs        ${yarp_msg_sources} ${ros_msg_sources}   PARENT_SCOPE)
set(cbw2_msgs_hrds        ${yarp_msg_headers} ${ros_msg_headers}   PARENT_SCOPE)
set(cbw2_msgs_incl_dirs   ${yarp_msg_include_dirs} ${ros_msg_include_dirs} PARENT_SCOPE)
