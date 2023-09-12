# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(ICHATBOTMSGS_THRIFT IChatBotMsgs.thrift)

add_library(IChatBotMsgs OBJECT)

target_sources(IChatBotMsgs PRIVATE ${ICHATBOTMSGS_THRIFT_GEN_FILES})

target_link_libraries(IChatBotMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IChatBotMsgs PUBLIC ${ICHATBOTMSGS_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IChatBotMsgs PROPERTY FOLDER "Devices/Shared")
