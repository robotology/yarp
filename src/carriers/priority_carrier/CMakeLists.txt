# Copyright (C) 2012 IITRBCS
# Authors: Ali Paikan and Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(priority_carrier TYPE PriorityCarrier INCLUDE PriorityCarrier.h)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_priority_carrier)

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  add_library(priority_carrier PriorityCarrier.h PriorityCarrier.cpp)
  target_link_libraries(priority_carrier ${YARP_LIBRARIES})
endif ()
