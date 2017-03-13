# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


# XmlRpc++
project(YARP_priv_xmlrpcpp)

set(xmlrpcpp_SRCS xmlrpcpp/src/base64.cpp
                  xmlrpcpp/src/XmlRpcClient.cpp
                  xmlrpcpp/src/XmlRpcServerConnection.cpp
                  xmlrpcpp/src/XmlRpcServer.cpp
                  xmlrpcpp/src/XmlRpcServerMethod.cpp
                  xmlrpcpp/src/XmlRpcSource.cpp
                  xmlrpcpp/src/XmlRpcUtil.cpp
                  xmlrpcpp/src/XmlRpcValue.cpp)

set(xmlrpcpp_HDRS xmlrpcpp/src/base64.h
                  xmlrpcpp/src/XmlRpcClient.h
                  xmlrpcpp/src/XmlRpcException.h
                  xmlrpcpp/src/XmlRpc.h
                  xmlrpcpp/src/XmlRpcServerConnection.h
                  xmlrpcpp/src/XmlRpcServer.h
                  xmlrpcpp/src/XmlRpcServerMethod.h
                  xmlrpcpp/src/XmlRpcSource.h
                  xmlrpcpp/src/XmlRpcUtil.h
                  xmlrpcpp/src/XmlRpcValue.h)

add_library(YARP_priv_xmlrpcpp STATIC ${xmlrpcpp_SRCS})

set_property(TARGET YARP_priv_xmlrpcpp PROPERTY FOLDER "Libraries/External")

set(xmlrpcpp_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/xmlrpcpp/src PARENT_SCOPE)
set(xmlrpcpp_LIBRARIES "YARP_priv_xmlrpcpp" PARENT_SCOPE)

install(TARGETS YARP_priv_xmlrpcpp
        EXPORT YARP
        COMPONENT runtime
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_priv_xmlrpcpp)
