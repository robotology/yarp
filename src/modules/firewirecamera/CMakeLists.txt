IF (UNIX)

  IF (COMPILE_DEVICE_LIBRARY)
    PREPARE_DEVICE(firewirecamera TYPE FirewireCamera INCLUDE FirewireCamera.h WRAPPER grabber)
  ENDIF (COMPILE_DEVICE_LIBRARY)

  IF (NOT SKIP_firewirecamera)
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
    FIND_PACKAGE(YARP REQUIRED)
    MESSAGE(STATUS "Warning, just taking it on faith that that raw1394 and dc1394_control exist")
    LINK_LIBRARIES(dc1394_control)
    ADD_LIBRARY(firewirecamera FirewireCamera.cpp FirewireCameraSet.cpp FirewireCameraSet.h dc1394settings.h FirewireCamera.h)

  ENDIF (NOT SKIP_firewirecamera)

ENDIF (UNIX)
