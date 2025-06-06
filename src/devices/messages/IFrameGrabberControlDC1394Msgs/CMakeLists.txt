# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(IFRAMEGRABBERCONTROLDC1394_THRIFT IFrameGrabberControlDC1394Msgs.thrift)

add_library(IFrameGrabberControlDC1394Msgs OBJECT)

target_sources(IFrameGrabberControlDC1394Msgs PRIVATE ${IFRAMEGRABBERCONTROLDC1394_THRIFT_GEN_FILES})

target_link_libraries(IFrameGrabberControlDC1394Msgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(IFrameGrabberControlDC1394Msgs PUBLIC ${IFRAMEGRABBERCONTROLDC1394_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET IFrameGrabberControlDC1394Msgs PROPERTY FOLDER "Plugins/Shared")
