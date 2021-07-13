# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(
  websocket
  CATEGORY carrier
  TYPE WebSocketCarrier
  INCLUDE WebSocketCarrier.h
  EXTRA_CONFIG
    CODE="GET /?ws"
)

if(NOT SKIP_websocket)
  yarp_add_plugin(yarp_websocket)

  set(WebSocket_SOURCES
    ${WebSocket_ROOT}/WebSocket/WebSocket.cpp
    ${WebSocket_ROOT}/WebSocket/WebSocket.h
    ${WebSocket_ROOT}/WebSocket/base64/base64.cpp
    ${WebSocket_ROOT}/WebSocket/base64/base64.h
    ${WebSocket_ROOT}/WebSocket/md5/md5.c
    ${WebSocket_ROOT}/WebSocket/md5/md5.h
    ${WebSocket_ROOT}/WebSocket/md5/md5.hpp
    ${WebSocket_ROOT}/WebSocket/sha1/sha1.cpp
    ${WebSocket_ROOT}/WebSocket/sha1/sha1.h
  )

  target_sources(yarp_websocket
    PRIVATE
      WebSocketCarrier.cpp
      WebSocketCarrier.h
      WebSocketStream.cpp
      WebSocketStream.h
      ${WebSocket_SOURCES}
  )

  set_source_files_properties(${WebSocket_SOURCES} PROPERTIES COMPILE_FLAGS "-w")

  target_include_directories(yarp_websocket
    PRIVATE
      ${WebSocket_ROOT}
  )

  target_link_libraries(yarp_websocket
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_wire_rep_utils
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
      YARP_os
      YARP_sig
      YARP_wire_rep_utils
  )

  yarp_install(
    TARGETS yarp_websocket
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_websocket PROPERTY FOLDER "Plugins/Carrier")
endif()
