# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS OR NOT TARGET YARP::YARP_math)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(FTM_THRIFT MobileBaseVelocityControlMsgs.thrift)

add_library(MobileBaseVelocityControlMsgs OBJECT)

target_sources(MobileBaseVelocityControlMsgs PRIVATE ${FTM_THRIFT_GEN_FILES})

target_link_libraries(MobileBaseVelocityControlMsgs
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_math
)

target_include_directories(MobileBaseVelocityControlMsgs PUBLIC ${FTM_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET MobileBaseVelocityControlMsgs PROPERTY FOLDER "Devices/Shared")
