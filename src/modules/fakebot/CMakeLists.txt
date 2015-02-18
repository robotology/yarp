# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(fakebot TYPE yarp::dev::FakeBot INCLUDE FakeBot.h)
  yarp_install(FILES fakebot.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_fakebot)

  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  yarp_add_plugin(yarp_fakebot FakeBot.cpp FakeBot.h)
  TARGET_LINK_LIBRARIES(yarp_fakebot ${YARP_LIBRARIES})
ENDIF (NOT SKIP_fakebot)

