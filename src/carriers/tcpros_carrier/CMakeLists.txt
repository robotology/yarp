# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(tcpros_carrier TYPE TcpRosCarrier INCLUDE TcpRosCarrier.h)
  yarp_prepare_carrier(rossrv_carrier TYPE RosSrvCarrier INCLUDE TcpRosCarrier.h)
  yarp_add_carrier_fingerprint(ros.ini rossrv_carrier tcpros_carrier)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_tcpros_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  get_property(WIRE_INCLUDE_DIRS TARGET wire_rep_utils PROPERTY INCLUDE_DIRS)
  include_directories(${WIRE_INCLUDE_DIRS})
  add_library(yarp_tcpros 
    TcpRosCarrier.h TcpRosCarrier.cpp
    TcpRosStream.h TcpRosStream.cpp
    RosHeader.h RosHeader.cpp
    RosSlave.h
    RosLookup.h RosLookup.cpp
    RosType.h RosType.cpp
    RosTypeCodeGenYarp.h RosTypeCodeGenYarp.cpp
  )
  target_link_libraries(yarp_tcpros wire_rep_utils)
  target_link_libraries(yarp_tcpros ${YARP_LIBRARIES})

  add_executable(yarpros yarpros.cpp)
  target_link_libraries(yarpros yarp_tcpros YARP_init ${YARP_LIBRARIES})
endif (NOT SKIP_tcpros_carrier)
