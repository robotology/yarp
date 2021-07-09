# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# XmlRpc++
project(YARP_priv_xmlrpcpp)

set(YARP_priv_xmlrpcpp_TYPE "STATIC" CACHE STRING "YARP_priv_xmlrpcpp (LGPL2.1+) library type (STATIC or SHARED)" )
set_property(CACHE YARP_priv_xmlrpcpp_TYPE PROPERTY STRINGS "STATIC" "SHARED")
mark_as_advanced(YARP_priv_xmlrpcpp_TYPE)
if(NOT YARP_priv_xmlrpcpp_TYPE MATCHES "(STATIC|SHARED)")
  message(WARNING "Invalid value for YARP_priv_xmlrpcpp_TYPE: ${YARP_priv_xmlrpcpp_TYPE}. Valid values are \"STATIC\" and \"SHARED\". Resetting to default value (STATIC).")
  set_property(CACHE YARP_priv_xmlrpcpp_TYPE PROPERTY VALUE "STATIC")
  unset(YARP_priv_xmlrpcpp_TYPE)
endif()

add_library(YARP_priv_xmlrpcpp ${YARP_priv_xmlrpcpp_TYPE})

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

target_sources(YARP_priv_xmlrpcpp PRIVATE  ${xmlrpcpp_SRCS})

target_include_directories(YARP_priv_xmlrpcpp PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/xmlrpcpp/src>)

set_property(TARGET YARP_priv_xmlrpcpp PROPERTY FOLDER "Libraries/External")

set(xmlrpcpp_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/xmlrpcpp/src PARENT_SCOPE)
set(xmlrpcpp_LIBRARIES "YARP_priv_xmlrpcpp" PARENT_SCOPE)

install(TARGETS YARP_priv_xmlrpcpp
        EXPORT YARP_priv_xmlrpcpp
        COMPONENT YARP_priv_xmlrpcpp
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

if(NOT CREATE_SHARED_LIBS)
  include(YarpInstallBasicPackageFiles)
  yarp_install_basic_package_files(YARP_priv_xmlrpcpp)
endif()
