# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_DEVICE_LIBRARY)
  YARP_PREPARE_DEVICE(fakebot TYPE FakeBot INCLUDE FakeBot.h)
  YARP_ADD_DEVICE_FINGERPRINT(fakebot.ini fakebot)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_fakebot)

  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  ADD_LIBRARY(yarp_fakebot FakeBot.cpp FakeBot.h)
  TARGET_LINK_LIBRARIES(yarp_fakebot ${YARP_LIBRARIES})
ENDIF (NOT SKIP_fakebot)

