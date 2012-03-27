# Copyright (C) 2012 IITRBCS
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  prepare_carrier(bayer_carrier TYPE BayerCarrier INCLUDE BayerCarrier.h)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_bayer_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  add_library(bayer_carrier BayerCarrier.h 
    BayerCarrier.cpp)
  target_link_libraries(bayer_carrier ${YARP_LIBRARIES})

endif ()
