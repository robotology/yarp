# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(opencv_grabber TYPE yarp::dev::OpenCVGrabber INCLUDE OpenCVGrabber.h WRAPPER grabber)
  yarp_install(FILES opencv.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_opencv_grabber)

    include_directories(${YARP_INCLUDE_DIRS})

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
    include_directories(${OpenCV_INCLUDE_DIRS})
    yarp_add_plugin(yarp_opencv OpenCVGrabber.cpp OpenCVGrabber.h)
    target_link_libraries(yarp_opencv ${YARP_LIBRARIES})
    target_link_libraries(yarp_opencv ${OpenCV_LIBRARIES})
    yarp_install(TARGETS yarp_opencv
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

    yarp_install(FILES opencv_grabber.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})

endif(NOT SKIP_opencv_grabber)
