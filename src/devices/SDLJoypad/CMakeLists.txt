# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(SDLJoypad
                    CATEGORY device
                    TYPE yarp::dev::SDLJoypad
                    INCLUDE SDLJoypad.h
                    EXTRA_CONFIG WRAPPER=JoypadControlServer
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_USE_SDL")

if(ENABLE_SDLJoypad)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  include_directories(SYSTEM ${SDL_INCLUDE_DIR})

  yarp_add_plugin(yarp_SDLJoypad SDLJoypad.cpp
                                 SDLJoypad.h)

  target_link_libraries(yarp_SDLJoypad YARP::YARP_OS
                                       YARP::YARP_sig
                                       YARP::YARP_dev
                                       ${SDL_LIBRARY})

  yarp_install(TARGETS yarp_SDLJoypad
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

  yarp_install(FILES SDLJoypad.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
