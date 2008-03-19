IF (COMPILE_DEVICE_LIBRARY)
  PREPARE_DEVICE(jrkerr TYPE JrkerrMotionControl INCLUDE JrkerrMotionControl.h WRAPPER controlboard)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_jrkerr)

FIND_PACKAGE(YARP REQUIRED)

IF (WIN32)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  ADD_LIBRARY(jrkerr JrkerrMotionControl.cpp nmccom.cpp picservo.cpp sio_util.cpp JrkerrMotionControl.h nmccom.h picservo.h sio_util.h)

ELSE (WIN32)

  MESSAGE(SEND_ERROR "not a system we know how to build jrkerr on yet")

ENDIF (WIN32)

ENDIF (NOT SKIP_jrkerr)

