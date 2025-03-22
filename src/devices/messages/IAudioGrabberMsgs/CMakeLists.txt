# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IAUDIOGRABBERMSG_THRIFT IAudioGrabberMsgs.thrift)

add_library(IAudioGrabberMsgs OBJECT)

target_sources(IAudioGrabberMsgs PRIVATE ${IAUDIOGRABBERMSG_THRIFT_GEN_FILES})

target_link_libraries(IAudioGrabberMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IAudioGrabberMsgs PUBLIC ${IAUDIOGRABBERMSG_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IAudioGrabberMsgs PROPERTY FOLDER "Plugins/Shared")
