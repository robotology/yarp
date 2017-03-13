# Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Nicolo' Genesio
# email:   nicolo.genesio@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT



yarp_prepare_plugin(test_grabber
                    CATEGORY device
                    TYPE yarp::dev::TestFrameGrabber
                    INCLUDE TestFrameGrabber.h
                    EXTRA_CONFIG WRAPPER=grabber
                    DEPENDS CREATE_DEVICE_LIBRARY_MODULES
                    DEFAULT ON)

if(ENABLE_test_grabber)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
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
