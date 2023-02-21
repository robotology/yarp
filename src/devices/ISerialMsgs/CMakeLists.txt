# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(ISERIALMSG_THRIFT ISerialMsgs.thrift)

add_library(ISerialMsgs OBJECT)

target_sources(ISerialMsgs PRIVATE ${ISERIALMSG_THRIFT_GEN_FILES})

target_link_libraries(ISerialMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(ISerialMsgs PUBLIC ${ISERIALMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET ISerialMsgs PROPERTY FOLDER "Devices/Shared")
