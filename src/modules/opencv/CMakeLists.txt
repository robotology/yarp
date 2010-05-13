# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
 
IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(opencv_grabber TYPE OpenCVGrabber INCLUDE OpenCVGrabber.h WRAPPER grabber)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_opencv_grabber)

FIND_PACKAGE(YARP REQUIRED)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

FIND_PACKAGE(OpenCV)

IF (OpenCV_FOUND)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common)
  INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
  LINK_LIBRARIES(${OpenCV_LIBRARIES})
  ADD_LIBRARY(yarp_opencv OpenCVGrabber.cpp OpenCVGrabber.h)


ELSE (OpenCV_FOUND)

  MESSAGE(SEND_ERROR "opencv not found")

ENDIF (OpenCV_FOUND)


ENDIF (NOT SKIP_opencv_grabber)
