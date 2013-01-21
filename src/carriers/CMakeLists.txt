# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

option(CREATE_OPTIONAL_CARRIERS "Compile some optional carriers" FALSE)
if (CREATE_OPTIONAL_CARRIERS)
  include(YarpPlugin)
  include_directories(${ACE_INCLUDE_DIRS})
  YARP_BEGIN_PLUGIN_LIBRARY(yarpcar)
    add_subdirectory(wire_rep_utils)
    add_subdirectory(human_carrier)
    add_subdirectory(mpi_carrier)
    add_subdirectory(xmlrpc_carrier)
    add_subdirectory(tcpros_carrier)
    add_subdirectory(mjpeg_carrier)
    add_subdirectory(bayer_carrier)
    add_subdirectory(priority_carrier)
  YARP_END_PLUGIN_LIBRARY(yarpcar)
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS yarpcar ${yarpcar_LIBRARIES})
  install(TARGETS yarpcar EXPORT YARP COMPONENT runtime DESTINATION lib${LIB_SUFFIX})
endif (CREATE_OPTIONAL_CARRIERS)
