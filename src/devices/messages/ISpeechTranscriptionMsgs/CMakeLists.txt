# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(ISPEECHTRANSCMSG_THRIFT ISpeechTranscriptionMsgs.thrift)

add_library(ISpeechTranscriptionMsgs OBJECT)

target_sources(ISpeechTranscriptionMsgs PRIVATE ${ISPEECHTRANSCMSG_THRIFT_GEN_FILES})

target_link_libraries(ISpeechTranscriptionMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(ISpeechTranscriptionMsgs PUBLIC ${ISPEECHTRANSCMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET ISpeechTranscriptionMsgs PROPERTY FOLDER "Plugins/Shared")
