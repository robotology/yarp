# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(fakeFrameGrabber
                    CATEGORY device
                    TYPE FakeFrameGrabber
                    INCLUDE FakeFrameGrabber.h
                    EXTRA_CONFIG WRAPPER=grabberDual
                    DEFAULT ON)

# As discussed in #2308 test_grabber should be renamed fakeFrameGrabber
#
# For now, we just add a warning, but in one of the next releases,
# test_grabber will become also a DeprecatedDevice, therefore it will not be
# possible to launch it without --allow-deprecated-devices
#
# When this is deprecated/removed, also the
# data/yarpmanager/tests/xml/modules/test_grabber.xml file should be updated
yarp_prepare_plugin(test_grabber
                    CATEGORY device
                    TYPE TestFrameGrabber
                    INCLUDE FakeFrameGrabber.h
                    EXTRA_CONFIG WRAPPER=grabberDual
                    DEFAULT ON)

if(ENABLE_fakeFrameGrabber)
  yarp_add_plugin(yarp_fakeFrameGrabber)

  target_sources(yarp_fakeFrameGrabber PRIVATE FakeFrameGrabber.cpp
                                               FakeFrameGrabber.h)

  target_link_libraries(yarp_fakeFrameGrabber PRIVATE YARP::YARP_os
                                                      YARP::YARP_sig
                                                      YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_fakeFrameGrabber
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeFrameGrabber PROPERTY FOLDER "Plugins/Device/Fake")
endif()
