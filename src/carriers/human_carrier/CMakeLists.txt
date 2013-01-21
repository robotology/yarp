# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_PLUGIN_LIBRARY)
  YARP_PREPARE_CARRIER(human_carrier TYPE HumanCarrier INCLUDE HumanCarrier.h)
ENDIF (COMPILE_PLUGIN_LIBRARY)

IF (NOT SKIP_human_carrier)
  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  yarp_add_plugin(human_carrier HumanCarrier.h HumanStream.h HumanCarrier.cpp HumanStream.cpp)
  TARGET_LINK_LIBRARIES(human_carrier ${YARP_LIBRARIES})
ENDIF (NOT SKIP_human_carrier)
