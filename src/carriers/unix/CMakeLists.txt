# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(unix_stream
                    CATEGORY carrier
                    TYPE UnixSocketCarrier
                    INCLUDE UnixSocketCarrier.h
                    EXTRA_CONFIG CODE=\"UNIX_\"\ \(any\)\ \(any\)\ \(any\)
                    DEPENDS UNIX
                    DEFAULT ON)

if(ENABLE_unix_stream OR ENABLE_unix_stream_ack)
  yarp_add_plugin(yarp_unix)

  target_sources(yarp_unix PRIVATE UnixSocketCarrier.cpp
                                   UnixSocketCarrier.h
                                   UnixSockTwoWayStream.cpp
                                   UnixSockTwoWayStream.h
                                   UnixSocketLogComponent.cpp
                                   UnixSocketLogComponent.h)

  target_link_libraries(yarp_unix YARP::YARP_os)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os)

  yarp_install(TARGETS yarp_unix
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_unix PROPERTY FOLDER "Plugins/Carrier")
endif()
