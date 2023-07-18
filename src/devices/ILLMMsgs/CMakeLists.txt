# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(ILLMMSGS_THRIFT ILLMMsgs.thrift)

add_library(ILLMMsgs OBJECT)

target_sources(ILLMMsgs PRIVATE ${ILLMMSGS_THRIFT_GEN_FILES})

target_link_libraries(ILLMMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(ILLMMsgs PUBLIC ${ILLMMSGS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET ILLMMsgs PROPERTY FOLDER "Devices/Shared")

