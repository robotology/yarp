IF (UNIX)

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(firewirecamera TYPE FirewireCamera INCLUDE common/yarp/FirewireCamera.h WRAPPER grabber)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  IF (NOT SKIP_firewirecamera)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/common)
    FIND_PACKAGE(YARP REQUIRED)
    MESSAGE(STATUS "Warning, just taking it on faith that that raw1394 and dc1394_control exist")
    ADD_LIBRARY(firewirecamera linux/yarp/FirewireCamera.cpp linux/yarp/FirewireCameraSet.cpp linux/yarp/FirewireCameraSet.h linux/yarp/dc1394settings.h common/yarp/FirewireCamera.h)

  ENDIF (NOT SKIP_firewirecamera)

ENDIF (UNIX)
