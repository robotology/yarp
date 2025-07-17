# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IFRAMEGRABBERCONTROL_THRIFT IFrameGrabberControlMsgs.thrift)

add_library(IFrameGrabberControlMsgs OBJECT)

target_sources(IFrameGrabberControlMsgs PRIVATE ${IFRAMEGRABBERCONTROL_THRIFT_GEN_FILES})

target_link_libraries(IFrameGrabberControlMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IFrameGrabberControlMsgs PUBLIC ${IFRAMEGRABBERCONTROL_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IFrameGrabberControlMsgs PROPERTY FOLDER "Plugins/Shared")
