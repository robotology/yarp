# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(xmlrpc
                    CATEGORY carrier
                    TYPE XmlRpcCarrier
                    INCLUDE XmlRpcCarrier.h
                    EXTRA_CONFIG CODE="POST /"
                    DEFAULT ON)

if(NOT SKIP_xmlrpc)
  yarp_add_plugin(yarp_xmlrpc)

  target_sources(yarp_xmlrpc PRIVATE XmlRpcCarrier.h
                                     XmlRpcCarrier.cpp
                                     XmlRpcStream.h
                                     XmlRpcStream.cpp
                                     XmlRpcLogComponent.h
                                     XmlRpcLogComponent.cpp)

  check_cxx_compiler_flag("-Wformat-nonliteral" CXX_HAS_WFORMAT_NONLITERAL)
  if(CXX_HAS_WFORMAT_NONLITERAL)
    set_source_files_properties(xmlrpc/XmlRpcValue.cpp PROPERTIES COMPILE_FLAGS "-Wno-format-nonliteral")
  endif()

  target_link_libraries(yarp_xmlrpc PRIVATE YARP::YARP_os)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os)

  target_include_directories(yarp_xmlrpc SYSTEM PRIVATE ${xmlrpcpp_INCLUDE_DIRS})
  target_link_libraries(yarp_xmlrpc PRIVATE ${xmlrpcpp_LIBRARIES})
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS xmlrpcpp)

  yarp_install(TARGETS yarp_xmlrpc
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_xmlrpc PROPERTY FOLDER "Plugins/Carrier")
endif()
