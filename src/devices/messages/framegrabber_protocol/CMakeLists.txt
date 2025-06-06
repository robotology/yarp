# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(YARP_COMPILE_DEVICE_PLUGINS)
  add_library(framegrabber_protocol OBJECT)

  target_sources(framegrabber_protocol
    PRIVATE
      yarp/proto/framegrabber/FrameGrabberOf_Forwarder.h
      yarp/proto/framegrabber/FrameGrabberOf_Forwarder-inl.h
      yarp/proto/framegrabber/FrameGrabberOf_Responder.h
      yarp/proto/framegrabber/FrameGrabberOf_Responder-inl.h
  )

  target_link_libraries(framegrabber_protocol
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )

  set_target_properties(framegrabber_protocol PROPERTIES LINKER_LANGUAGE CXX)

  target_include_directories(framegrabber_protocol PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

  set_property(TARGET framegrabber_protocol PROPERTY FOLDER "Plugins/Shared")
endif()
