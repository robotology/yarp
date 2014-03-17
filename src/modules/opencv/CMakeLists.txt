# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(opencv_grabber TYPE OpenCVGrabber INCLUDE OpenCVGrabber.h WRAPPER grabber)
    yarp_add_device_fingerprint(opencv.ini opencv_grabber)
endif(COMPILE_DEVICE_LIBRARY)

if(NOT SKIP_opencv_grabber)

    include_directories(${YARP_INCLUDE_DIRS})

    find_package(OpenCV REQUIRED)

    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
    include_directories(${OpenCV_INCLUDE_DIRS})
    yarp_add_plugin(yarp_opencv OpenCVGrabber.cpp OpenCVGrabber.h)
    target_link_libraries(yarp_opencv LINK_PRIVATE ${YARP_LIBRARIES})
    target_link_libraries(yarp_opencv LINK_PRIVATE ${ACE_LIBRARIES})
    target_link_libraries(yarp_opencv LINK_PRIVATE ${OpenCV_LIBRARIES})

    yarp_install(FILES opencv_grabber.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})

endif(NOT SKIP_opencv_grabber)
