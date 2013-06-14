# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(opencv_grabber TYPE OpenCVGrabber INCLUDE OpenCVGrabber.h WRAPPER grabber)
  YARP_ADD_DEVICE_FINGERPRINT(opencv.ini opencv_grabber)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_opencv_grabber)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

FIND_PACKAGE(OpenCV REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common)
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
yarp_add_plugin(yarp_opencv OpenCVGrabber.cpp OpenCVGrabber.h)
target_link_libraries(yarp_opencv LINK_PRIVATE ${YARP_LIBRARIES})
target_link_libraries(yarp_opencv LINK_PRIVATE ${ACE_LIBRARIES})
target_link_libraries(yarp_opencv LINK_PRIVATE ${OpenCV_LIBRARIES})

configure_file(opencv_grabber.xml ${CMAKE_BINARY_DIR}/modules/opencv_grabber.xml COPYONLY)
if(YARP_TREE_INCLUDE_DIRS)
install(FILES ${CMAKE_BINARY_DIR}/modules/opencv_grabber.xml
        DESTINATION share/yarp/modules)
endif(YARP_TREE_INCLUDE_DIRS)

ENDIF (NOT SKIP_opencv_grabber)
