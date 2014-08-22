# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_PLUGIN_LIBRARY)
  YARP_PREPARE_CARRIER(human_carrier TYPE HumanCarrier INCLUDE HumanCarrier.h)
  YARP_ADD_CARRIER_FINGERPRINT(human.ini human_carrier)
endif()

if(NOT SKIP_human_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  yarp_add_plugin(yarp_human HumanCarrier.h HumanStream.h HumanCarrier.cpp HumanStream.cpp)
  target_link_libraries(yarp_human YARP_OS YARP_sig)
  target_link_libraries(yarp_human ${ACE_LIBRARIES})
endif()
