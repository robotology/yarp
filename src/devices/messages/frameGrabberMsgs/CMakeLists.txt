# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(FRAMEGRABBER_THRIFT frameGrabberMsgs.thrift)

add_library(FrameGrabberMsgs OBJECT)

target_sources(FrameGrabberMsgs PRIVATE ${FRAMEGRABBER_THRIFT_GEN_FILES})

target_link_libraries(FrameGrabberMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(FrameGrabberMsgs PUBLIC ${FRAMEGRABBER_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET FrameGrabberMsgs PROPERTY FOLDER "Plugins/Shared")
