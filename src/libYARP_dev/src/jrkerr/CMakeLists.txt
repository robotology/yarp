FIND_PACKAGE(YARP REQUIRED)

IF (WIN32)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/winnt)

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(jrkerr TYPE JrkerrMotionControl INCLUDE winnt/yarp/JrkerrMotionControl.h WRAPPER controlboard)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  ADD_LIBRARY(jrkerr winnt/yarp/JrkerrMotionControl.cpp winnt/dd_orig/src/nmccom.cpp winnt/dd_orig/src/picservo.cpp winnt/dd_orig/src/sio_util.cpp winnt/yarp/JrkerrMotionControl.h winnt/dd_orig/include/nmccom.h winnt/dd_orig/include/picservo.h winnt/dd_orig/include/sio_util.h)

ELSE (WIN32)

  MESSAGE(STATUS "not a system we know how to build jrkerr on yet")

ENDIF (WIN32)


