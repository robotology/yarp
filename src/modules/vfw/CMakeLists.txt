# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# If being compiled as part of a library, create a "factory" for each
# of our devices - just one in this case, "vfw_grabber", created by
# making an instance of VfwGrabber (declared in VfwGrabber.h).
# Our devices can optionally be declared to have a network wrapper.
IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(vfw_grabber TYPE VfwGrabber INCLUDE VfwGrabber.h WRAPPER grabber)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_vfw_grabber)

  # Try to find all the packages/libraries we need
  FIND_PACKAGE(YARP REQUIRED)
  FIND_LIBRARY(vfw_library vfw32)

  IF (vfw_library)

    # Set up any paths, link any libraries we need
    INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
    LINK_LIBRARIES(${vfw_library})

    # Set up any compiler definitions needed
    IF (MINGW)
      # Need a small fix on MinGW, some missing declarations.
      ADD_DEFINITIONS(-D__MINGW__)
    ENDIF (MINGW)

    # Create a library of our device code.
    ADD_LIBRARY(vfw_grabber VfwGrabber.cpp VfwGrabber.h)

    # If not being compiled as part of a library, try making a test program.
    IF (NOT COMPILE_DEVICE_LIBRARY)
      LINK_LIBRARIES(vfw_grabber)
      ADD_EXECUTABLE(vfw_test vfw_test.cpp)
    ENDIF (NOT COMPILE_DEVICE_LIBRARY)

  ELSE (vfw_library)

    # If we cannot build, we should proceed gracefully - it is not an error.
    MESSAGE(STATUS "cannot find video for windows")

  ENDIF (vfw_library)

ENDIF (NOT SKIP_vfw_grabber)

