# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

IF (COMPILE_PLUGIN_LIBRARY)
  YARP_PREPARE_CARRIER(human_carrier TYPE HumanCarrier INCLUDE HumanCarrier.h)
  YARP_ADD_CARRIER_FINGERPRINT(human.ini human_carrier)
ENDIF (COMPILE_PLUGIN_LIBRARY)

IF (NOT SKIP_human_carrier)
  FIND_PACKAGE(YARP REQUIRED)
  INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
  yarp_add_plugin(yarp_human HumanCarrier.h HumanStream.h HumanCarrier.cpp HumanStream.cpp)
  target_link_libraries(yarp_human YARP_OS YARP_sig)
  target_link_libraries(yarp_human ${ACE_LIBRARIES})
ENDIF (NOT SKIP_human_carrier)
