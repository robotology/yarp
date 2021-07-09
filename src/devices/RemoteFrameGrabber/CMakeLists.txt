# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(remote_grabber
  CATEGORY device
  TYPE RemoteFrameGrabber
  INCLUDE RemoteFrameGrabber.h
  EXTRA_CONFIG WRAPPER=grabber
  DEFAULT ON
)

if(NOT SKIP_remote_grabber)
  yarp_add_plugin(yarp_remote_grabber)

  target_sources(yarp_remote_grabber
    PRIVATE
      RemoteFrameGrabber.cpp
      RemoteFrameGrabber.h
  )

  target_sources(yarp_remote_grabber PRIVATE $<TARGET_OBJECTS:framegrabber_protocol>)
  target_include_directories(yarp_remote_grabber PRIVATE $<TARGET_PROPERTY:framegrabber_protocol,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_remote_grabber
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_remote_grabber
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_remote_grabber PROPERTY FOLDER "Plugins/Device")
endif()


yarp_prepare_plugin(frameGrabber_nwc_yarp
  CATEGORY device
  TYPE FrameGrabber_nwc_yarp
  INCLUDE FrameGrabber_nwc_yarp.h
  EXTRA_CONFIG WRAPPER=frameGrabber_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_frameGrabber_nwc_yarp)
  yarp_add_plugin(yarp_frameGrabber_nwc_yarp)

  target_sources(yarp_frameGrabber_nwc_yarp
    PRIVATE
      FrameGrabber_nwc_yarp.cpp
      FrameGrabber_nwc_yarp.h
    )

  target_sources(yarp_frameGrabber_nwc_yarp PRIVATE $<TARGET_OBJECTS:framegrabber_protocol>)
  target_include_directories(yarp_frameGrabber_nwc_yarp PRIVATE $<TARGET_PROPERTY:framegrabber_protocol,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameGrabber_nwc_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_frameGrabber_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameGrabber_nwc_yarp PROPERTY FOLDER "Plugins/Device")
endif()
