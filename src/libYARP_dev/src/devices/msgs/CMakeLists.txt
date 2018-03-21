# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include(YarpIDL)

yarp_idl_to_dir("${CMAKE_CURRENT_SOURCE_DIR}/stateExt.thrift"
                "${CMAKE_CURRENT_SOURCE_DIR}/yarp"
                yarp_msg_sources
                yarp_msg_headers
                yarp_msg_include_dirs)
include("${CMAKE_CURRENT_SOURCE_DIR}/yarp/stateExt_thrift.cmake")

# # add automatically generated files to source list
set(cbw2_msgs_srcs        ${yarp_msg_sources} ${ros_msg_sources}   PARENT_SCOPE)
set(cbw2_msgs_hrds        ${yarp_msg_headers} ${ros_msg_headers}   PARENT_SCOPE)
set(cbw2_msgs_incl_dirs   ${yarp_msg_include_dirs} ${ros_msg_include_dirs} PARENT_SCOPE)
