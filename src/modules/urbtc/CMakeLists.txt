# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 
IF (COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(urbtc TYPE yarp::dev::UrbtcControl INCLUDE UrbtcControl.h WRAPPER controlboard)
ENDIF (COMPILE_DEVICE_LIBRARY)

IF (NOT SKIP_urbtc)

FIND_PACKAGE(YARP REQUIRED)

IF (UNIX)
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

  yarp_add_plugin(urbtc UrbtcControl.cpp UrbtcControl.h)
  yarp_install(TARGETS urbtc
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

ELSE (UNIX)

  MESSAGE(SEND_ERROR "urbtc is for unix")

ENDIF (UNIX)

ENDIF (NOT SKIP_urbtc)

