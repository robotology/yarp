# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(ServerSoundGrabber
                    CATEGORY device
                    TYPE yarp::dev::ServerSoundGrabber
                    INCLUDE ServerSoundGrabber.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES"
                    DEFAULT ON)

if(NOT SKIP_ServerSoundGrabber)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_ServerSoundGrabber ServerSoundGrabber.h)
  target_link_libraries(yarp_ServerSoundGrabber YARP::YARP_OS
                                                YARP::YARP_sig
                                                YARP::YARP_dev)
  yarp_install(TARGETS yarp_ServerSoundGrabber
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES ServerSoundGrabber.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_ServerSoundGrabber PROPERTY FOLDER "Plugins/Device")
endif()
