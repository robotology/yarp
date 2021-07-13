# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_DEVICE_PLUGINS)
  add_library(framegrabber_protocol OBJECT)

  target_sources(framegrabber_protocol
    PRIVATE
      yarp/proto/framegrabber/DepthVisualParams_Responder.cpp
      yarp/proto/framegrabber/DepthVisualParams_Responder.h
      yarp/proto/framegrabber/DepthVisualParams_Forwarder.cpp
      yarp/proto/framegrabber/DepthVisualParams_Forwarder.h
      yarp/proto/framegrabber/FrameGrabberControlsDC1394_Responder.cpp
      yarp/proto/framegrabber/FrameGrabberControlsDC1394_Responder.h
      yarp/proto/framegrabber/FrameGrabberControlsDC1394_Forwarder.cpp
      yarp/proto/framegrabber/FrameGrabberControlsDC1394_Forwarder.h
      yarp/proto/framegrabber/FrameGrabberControls_Responder.cpp
      yarp/proto/framegrabber/FrameGrabberControls_Responder.h
      yarp/proto/framegrabber/FrameGrabberControls_Forwarder.cpp
      yarp/proto/framegrabber/FrameGrabberControls_Forwarder.h
      yarp/proto/framegrabber/FrameGrabberOf_Forwarder.h
      yarp/proto/framegrabber/FrameGrabberOf_Forwarder-inl.h
      yarp/proto/framegrabber/FrameGrabberOf_Responder.h
      yarp/proto/framegrabber/FrameGrabberOf_Responder-inl.h
      yarp/proto/framegrabber/RgbVisualParams_Responder.cpp
      yarp/proto/framegrabber/RgbVisualParams_Responder.h
      yarp/proto/framegrabber/RgbVisualParams_Forwarder.cpp
      yarp/proto/framegrabber/RgbVisualParams_Forwarder.h
  )

  target_link_libraries(framegrabber_protocol
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )

  target_include_directories(framegrabber_protocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

  set_property(TARGET framegrabber_protocol PROPERTY FOLDER "Devices/Shared")
endif()
