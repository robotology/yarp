# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  prepare_carrier(xmlrpc_carrier TYPE XmlRpcCarrier INCLUDE XmlRpcCarrier.h)
  yarp_add_carrier_fingerprint(xmlrpc.ini xmlrpc_carrier)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_xmlrpc_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xmlrpc)
  add_library(yarp_xmlrpc
    XmlRpcCarrier.h XmlRpcCarrier.cpp
    XmlRpcStream.h XmlRpcStream.cpp
    xmlrpc/XmlRpcClient.cpp
    xmlrpc/XmlRpcServer.cpp
    xmlrpc/XmlRpcServerConnection.cpp
    xmlrpc/XmlRpcServerMethod.cpp
    xmlrpc/XmlRpcSource.cpp
    xmlrpc/XmlRpcUtil.cpp
    xmlrpc/XmlRpcValue.cpp)
  target_link_libraries(yarp_xmlrpc ${YARP_LIBRARIES})
endif (NOT SKIP_xmlrpc_carrier)
