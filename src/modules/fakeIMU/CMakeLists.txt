# Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
# Authors: Alberto Cardellino
# email:   alberto.cardellino@iit.it
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(fakeIMU TYPE yarp::dev::fakeIMU INCLUDE fakeIMU.h WRAPPER inertial)
  yarp_install(FILES fakeIMU.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_fakeIMU)

  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  yarp_add_plugin(yarp_fakeIMU fakeIMU.cpp fakeIMU.h)
  TARGET_LINK_LIBRARIES(yarp_fakeIMU ${YARP_LIBRARIES})
  yarp_install(TARGETS yarp_fakeIMU
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

ENDIF (NOT SKIP_fakeIMU)

