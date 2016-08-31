# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_plugin(tcpros_carrier
                      CATEGORY carrier
                      TYPE yarp::os::TcpRosCarrier
                      INCLUDE TcpRosCarrier.h)
  yarp_prepare_plugin(rossrv_carrier
                      CATEGORY carrier
                      TYPE yarp::os::RosSrvCarrier
                      INCLUDE TcpRosCarrier.h)
  yarp_install(FILES ros.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if(NOT SKIP_tcpros_carrier)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_wire_rep_utils_INCLUDE_DIRS TARGET YARP_wire_rep_utils PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_wire_rep_utils_INCLUDE_DIRS})
  yarp_add_plugin(yarp_tcpros
                  TcpRosCarrier.h
                  TcpRosCarrier.cpp
                  TcpRosStream.h
                  TcpRosStream.cpp
                  RosHeader.h
                  RosHeader.cpp
                  RosSlave.h
                  RosLookup.h
                  RosLookup.cpp)
  target_link_libraries(yarp_tcpros YARP_OS
                                    YARP_sig
                                    YARP_wire_rep_utils)
  yarp_install(TARGETS yarp_tcpros
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  add_executable(yarpros yarpros.cpp
                         TcpRosStream.cpp
                         RosLookup.cpp)
  target_link_libraries(yarpros YARP_OS
                                YARP_init
                                YARP_wire_rep_utils)
  install(TARGETS yarpros
          EXPORT YARP
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
