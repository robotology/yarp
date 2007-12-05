
FIND_PACKAGE(YARP REQUIRED)

IF (WINDOWS OR MINGW)

  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  LINK_LIBRARIES(vfw32)
  ADD_LIBRARY(vfw_grabber VfwGrabber.cpp)
  LINK_LIBRARIES(vfw_grabber)
  ADD_EXECUTABLE(vfw_test vfw_test.cpp)

ELSE (WINDOWS OR MINGW)

  MESSAGE(STATUS "not a platform we know to support video for windows")

ENDIF (WINDOWS OR MINGW)
