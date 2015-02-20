# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(xmlrpc_carrier TYPE yarp::os::XmlRpcCarrier INCLUDE XmlRpcCarrier.h)
  yarp_install(FILES xmlrpc.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_xmlrpc_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xmlrpc)
  yarp_add_plugin(yarp_xmlrpc
                  XmlRpcCarrier.h XmlRpcCarrier.cpp
                  XmlRpcStream.h XmlRpcStream.cpp
                  xmlrpc/XmlRpcClient.cpp
                  xmlrpc/XmlRpcServer.cpp
                  xmlrpc/XmlRpcServerConnection.cpp
                  xmlrpc/XmlRpcServerMethod.cpp
                  xmlrpc/XmlRpcSource.cpp
                  xmlrpc/XmlRpcUtil.cpp
                  xmlrpc/XmlRpcValue.cpp)
  target_link_libraries(yarp_xmlrpc YARP_OS YARP_sig)
  target_link_libraries(yarp_xmlrpc ${ACE_LIBRARIES})
  yarp_install(TARGETS yarp_xmlrpc
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif (NOT SKIP_xmlrpc_carrier)
