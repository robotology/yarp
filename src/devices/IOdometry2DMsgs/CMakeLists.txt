# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IODOMETRY2DMSG_THRIFT IOdometry2DMsgs.thrift)

add_library(IOdometry2DMsgs OBJECT)

target_sources(IOdometry2DMsgs PRIVATE ${IODOMETRY2DMSG_THRIFT_GEN_FILES})

target_link_libraries(IOdometry2DMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IOdometry2DMsgs PUBLIC ${IODOMETRY2DMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IOdometry2DMsgs PROPERTY FOLDER "Devices/Shared")
