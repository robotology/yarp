# Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Author: Alberto Cardellino
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(fakeMotionControl TYPE yarp::dev::FakeMotionControl INCLUDE fakeMotionControl.h)
  yarp_install(FILES fakeMotionControl.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_fakeMotionControl)

  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  yarp_add_plugin(yarp_fakeMotionControl fakeMotionControl.cpp fakeMotionControl.h)
  TARGET_LINK_LIBRARIES(yarp_fakeMotionControl ${YARP_LIBRARIES})
  yarp_install(TARGETS yarp_fakeMotionControl
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

ENDIF (NOT SKIP_fakeMotionControl)

