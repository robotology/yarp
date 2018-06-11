# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(test_grabber
                    CATEGORY device
                    TYPE yarp::dev::TestFrameGrabber
                    INCLUDE TestFrameGrabber.h
                    EXTRA_CONFIG WRAPPER=grabber
                    DEFAULT ON)

if(ENABLE_test_grabber)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_test_grabber TestFrameGrabber.cpp
                                        TestFrameGrabber.h)
  target_link_libraries(yarp_test_grabber YARP_OS
                                          YARP_sig
                                          YARP_dev)
  yarp_install(TARGETS yarp_test_grabber
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES test_grabber.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_test_grabber PROPERTY FOLDER "Plugins/Device/Fake")
endif()
