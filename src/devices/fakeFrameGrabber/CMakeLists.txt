# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(fakeFrameGrabber
  CATEGORY device
  TYPE FakeFrameGrabber
  INCLUDE FakeFrameGrabber.h
  EXTRA_CONFIG
    WRAPPER=grabberDual
  DEFAULT ON
)

yarp_prepare_plugin(test_grabber
  CATEGORY device
  TYPE TestFrameGrabber
  INCLUDE FakeFrameGrabber.h
  EXTRA_CONFIG
    WRAPPER=grabberDual
  DEFAULT OFF
  DEPENDS "NOT YARP_NO_DEPRECATED") # DEPRECATED Since YARP 3.5

if(ENABLE_fakeFrameGrabber)
  yarp_add_plugin(yarp_fakeFrameGrabber)

  target_sources(yarp_fakeFrameGrabber
    PRIVATE
      FakeFrameGrabber.cpp
      FakeFrameGrabber.h
  )

  target_link_libraries(yarp_fakeFrameGrabber
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
    TARGETS yarp_fakeFrameGrabber
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeFrameGrabber PROPERTY FOLDER "Plugins/Device/Fake")
endif()
