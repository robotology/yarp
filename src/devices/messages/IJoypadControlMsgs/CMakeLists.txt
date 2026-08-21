# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IJOYPADCONTROLMSGS_THRIFT IJoypadControlMsgs.thrift)

add_library(IJoypadControlMsgs OBJECT)

target_sources(IJoypadControlMsgs PRIVATE ${IJOYPADCONTROLMSGS_THRIFT_GEN_FILES})

target_link_libraries(IJoypadControlMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IJoypadControlMsgs PUBLIC ${IJOYPADCONTROLMSGS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IJoypadControlMsgs PROPERTY FOLDER "Plugins/Shared")
