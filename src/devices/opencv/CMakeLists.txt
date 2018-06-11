# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(opencv_grabber
                    CATEGORY device
                    TYPE yarp::dev::OpenCVGrabber
                    INCLUDE OpenCVGrabber.h
                    EXTRA_CONFIG WRAPPER=grabber
                    DEPENDS "YARP_HAS_OPENCV")

if(NOT SKIP_opencv_grabber)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})

  yarp_add_plugin(yarp_opencv OpenCVGrabber.cpp
                              OpenCVGrabber.h)
  target_link_libraries(yarp_opencv YARP::YARP_OS
                                    YARP::YARP_sig
                                    YARP::YARP_dev
                                    ${OpenCV_LIBRARIES})
  yarp_install(TARGETS yarp_opencv
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES opencv.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  yarp_install(FILES opencv_grabber.xml
               DESTINATION ${YARP_MODULES_INSTALL_DIR})

  set_property(TARGET yarp_opencv PROPERTY FOLDER "Plugins/Device")
endif()
