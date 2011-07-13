# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  prepare_carrier(xmlrpc_carrier TYPE XmlRpcCarrier INCLUDE XmlRpcCarrier.h)
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_xmlrpc_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  add_library(xmlrpc_carrier XmlRpcCarrier.h XmlRpcCarrier.cpp
    XmlRpcStream.h XmlRpcStream.cpp)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xmlrpc)
  add_library(xmlrpcpp xmlrpc/XmlRpcClient.cpp
                       xmlrpc/XmlRpcDispatch.cpp
                       xmlrpc/XmlRpcServer.cpp
                       xmlrpc/XmlRpcServerConnection.cpp
                       xmlrpc/XmlRpcServerMethod.cpp
                       xmlrpc/XmlRpcSocket.cpp
                       xmlrpc/XmlRpcSource.cpp
                       xmlrpc/XmlRpcUtil.cpp
                       xmlrpc/XmlRpcValue.cpp)
  target_link_libraries(xmlrpc_carrier xmlrpcpp ${YARP_LIBRARIES})
endif (NOT SKIP_xmlrpc_carrier)
