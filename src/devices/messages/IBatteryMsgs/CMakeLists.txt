# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IBATTERYMSG_THRIFT IBatteryMsgs.thrift)

add_library(IBatteryMsgs OBJECT)

target_sources(IBatteryMsgs PRIVATE ${IBATTERYMSG_THRIFT_GEN_FILES})

target_link_libraries(IBatteryMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IBatteryMsgs PUBLIC ${IBATTERYMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IBatteryMsgs PROPERTY FOLDER "Plugins/Shared")
