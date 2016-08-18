# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(opencv_grabber
                      TYPE yarp::dev::OpenCVGrabber
                      INCLUDE OpenCVGrabber.h
                      WRAPPER grabber)
  yarp_install(FILES opencv.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_opencv_grabber)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})

  yarp_add_plugin(yarp_opencv OpenCVGrabber.cpp
                              OpenCVGrabber.h)
  target_link_libraries(yarp_opencv YARP_OS
                                    YARP_sig
                                    YARP_dev
                                    ${OpenCV_LIBRARIES})
    yarp_install(TARGETS yarp_opencv
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES opencv_grabber.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})

endif(NOT SKIP_opencv_grabber)
