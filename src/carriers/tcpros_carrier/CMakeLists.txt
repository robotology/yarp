# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(tcpros_carrier TYPE yarp::os::TcpRosCarrier INCLUDE TcpRosCarrier.h)
  yarp_prepare_carrier(rossrv_carrier TYPE yarp::os::RosSrvCarrier INCLUDE TcpRosCarrier.h)
  yarp_add_carrier_fingerprint(ros.ini rossrv_carrier tcpros_carrier)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_tcpros_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  get_property(WIRE_INCLUDE_DIRS TARGET YARP_wire_rep_utils PROPERTY INCLUDE_DIRS)
  include_directories(${WIRE_INCLUDE_DIRS})
  yarp_add_plugin(yarp_tcpros
                  TcpRosCarrier.h TcpRosCarrier.cpp
                  TcpRosStream.h TcpRosStream.cpp
                  RosHeader.h RosHeader.cpp
                  RosSlave.h
                  RosLookup.h RosLookup.cpp)
  target_link_libraries(yarp_tcpros YARP_wire_rep_utils)
  target_link_libraries(yarp_tcpros YARP_OS YARP_sig)
  target_link_libraries(yarp_tcpros ${ACE_LIBRARIES})

  add_executable(yarpros yarpros.cpp
                         TcpRosStream.cpp
                         RosLookup.cpp)
  target_link_libraries(yarpros YARP_wire_rep_utils)
  target_link_libraries(yarpros YARP_OS YARP_init)
  target_link_libraries(yarpros ${ACE_LIBRARIES})
  install(TARGETS yarpros COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})
endif (NOT SKIP_tcpros_carrier)
