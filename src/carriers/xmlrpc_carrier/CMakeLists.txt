# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(xmlrpc
                    CATEGORY carrier
                    TYPE yarp::os::XmlRpcCarrier
                    INCLUDE XmlRpcCarrier.h
                    EXTRA_CONFIG CODE="POST /"
                    DEPENDS "CREATE_OPTIONAL_CARRIERS;YARP_HAS_ACE"
                    DEFAULT ON)

if(NOT SKIP_xmlrpc_carrier)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/xmlrpc)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS})

  check_cxx_compiler_flag("-Wformat-nonliteral" CXX_HAS_WFORMAT_NONLITERAL)
  if(CXX_HAS_WFORMAT_NONLITERAL)
    set_source_files_properties(xmlrpc/XmlRpcValue.cpp PROPERTIES COMPILE_FLAGS "-Wno-format-nonliteral")
  endif()

  yarp_add_plugin(yarp_xmlrpc
                  XmlRpcCarrier.h
                  XmlRpcCarrier.cpp
                  XmlRpcStream.h
                  XmlRpcStream.cpp
                  xmlrpc/XmlRpcClient.cpp
                  xmlrpc/XmlRpcServer.cpp
                  xmlrpc/XmlRpcServerConnection.cpp
                  xmlrpc/XmlRpcServerMethod.cpp
                  xmlrpc/XmlRpcSource.cpp
                  xmlrpc/XmlRpcUtil.cpp
                  xmlrpc/XmlRpcValue.cpp)
  target_link_libraries(yarp_xmlrpc YARP_OS)
  yarp_install(TARGETS yarp_xmlrpc
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES xmlrpc.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
