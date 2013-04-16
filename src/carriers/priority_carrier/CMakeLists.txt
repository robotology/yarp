# Copyright (C) 2012 IITRBCS
# Authors: Ali Paikan and Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(priority_carrier TYPE PriorityCarrier INCLUDE PriorityCarrier.h)
  yarp_add_carrier_fingerprint(priority.ini priority_carrier)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_priority_carrier)

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})

  yarp_add_plugin(yarp_priority PriorityCarrier.h PriorityCarrier.cpp)

  if(CREATE_LIB_MATH)
    ADD_DEFINITIONS( -DWITH_YARPMATH )
    target_link_libraries(yarp_priority YARP_OS YARP_sig YARP_math)
  else(CREATE_LIB_MATH)
     target_link_libraries(yarp_priority YARP_OS YARP_sig)
  endif(CREATE_LIB_MATH)

  target_link_libraries(yarp_priority ${ACE_LIBRARIES})

endif ()
