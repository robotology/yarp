# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(CONTROLBOARDMSG_THRIFT ControlBoardMsgs.thrift)

add_library(ControlBoardMsgs OBJECT)

target_sources(ControlBoardMsgs PRIVATE ${CONTROLBOARDMSG_THRIFT_GEN_FILES})

target_link_libraries(ControlBoardMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(ControlBoardMsgs PUBLIC ${CONTROLBOARDMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET ControlBoardMsgs PROPERTY FOLDER "Plugins/Shared")
