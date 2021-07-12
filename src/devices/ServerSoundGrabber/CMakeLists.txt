# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(ServerSoundGrabber
                    CATEGORY device
                    TYPE ServerSoundGrabber
                    INCLUDE ServerSoundGrabber.h
                    DEFAULT OFF
                    DEPENDS "NOT YARP_NO_DEPRECATED") # DEPRECATED Since YARP 3.2

if(NOT SKIP_ServerSoundGrabber)
  yarp_add_plugin(yarp_ServerSoundGrabber)

  target_sources(yarp_ServerSoundGrabber PRIVATE ServerSoundGrabber.cpp
                                                 ServerSoundGrabber.h)

  target_link_libraries(yarp_ServerSoundGrabber PRIVATE YARP::YARP_os
                                                        YARP::YARP_sig
                                                        YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_ServerSoundGrabber
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_ServerSoundGrabber PROPERTY FOLDER "Plugins/Device")
endif()
