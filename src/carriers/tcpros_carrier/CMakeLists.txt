# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(tcpros
                    CATEGORY carrier
                    TYPE yarp::os::TcpRosCarrier
                    INCLUDE TcpRosCarrier.h
                    EXTRA_CONFIG CODE=\(any\)\ \(any\)\ \"\\\\0\\\\0\"\ \(any\)\ \(any\)\ \"\\\\0\\\\0\"
                    DEPENDS "CREATE_OPTIONAL_CARRIERS;YARP_HAS_ACE"
                    DEFAULT ON)

yarp_prepare_plugin(rossrv
                    CATEGORY carrier
                    TYPE yarp::os::RosSrvCarrier
                    INCLUDE TcpRosCarrier.h
                    EXTRA_CONFIG CODE=\(any\)\ \(any\)\ \"\\\\0\\\\0\"\ \(any\)\ \(any\)\ \"\\\\0\\\\0\"
                    DEPENDS "CREATE_OPTIONAL_CARRIERS;YARP_HAS_ACE"
                    DEFAULT ON)

if(NOT SKIP_tcpros OR NOT SKIP_rossrv)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
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
  target_link_libraries(yarp_tcpros YARP::YARP_OS
                                    YARP::YARP_sig
                                    YARP::YARP_wire_rep_utils)
  yarp_install(TARGETS yarp_tcpros
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES ros.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_tcpros PROPERTY FOLDER "Plugins/Carrier")

  add_executable(yarpros yarpros.cpp
                         TcpRosStream.cpp
                         RosLookup.cpp)
  target_link_libraries(yarpros YARP::YARP_OS
                                YARP::YARP_init
                                YARP::YARP_wire_rep_utils)
  install(TARGETS yarpros
          EXPORT YARP
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpros PROPERTY FOLDER "Command Line Tools")
endif()
