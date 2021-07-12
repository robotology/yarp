# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(grabber
                    CATEGORY device
                    TYPE ServerFrameGrabber
                    INCLUDE ServerFrameGrabber.h
                    EXTRA_CONFIG WRAPPER=grabber
                    DEFAULT OFF
                    DEPENDS "NOT YARP_NO_DEPRECATED") # DEPRECATED Since YARP 3.3

if(NOT SKIP_grabber)
  yarp_add_plugin(yarp_grabber)

  target_sources(yarp_grabber PRIVATE ServerFrameGrabber.cpp
                                      ServerFrameGrabber.h)

  target_sources(yarp_grabber PRIVATE $<TARGET_OBJECTS:framegrabber_protocol>)
  target_include_directories(yarp_grabber PRIVATE $<TARGET_PROPERTY:framegrabber_protocol,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_grabber PRIVATE YARP::YARP_os
                                             YARP::YARP_sig
                                             YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_grabber
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_grabber PROPERTY FOLDER "Plugins/Device")
endif()
