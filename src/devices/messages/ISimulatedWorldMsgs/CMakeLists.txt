# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(ISIMULATEDWORLDMSG_THRIFT ISimulatedWorldMsgs.thrift)

add_library(ISimulatedWorldMsgs OBJECT)

target_sources(ISimulatedWorldMsgs PRIVATE ${ISIMULATEDWORLDMSG_THRIFT_GEN_FILES})

target_link_libraries(ISimulatedWorldMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(ISimulatedWorldMsgs PUBLIC ${ISIMULATEDWORLDMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET ISimulatedWorldMsgs PROPERTY FOLDER "Plugins/Shared")
