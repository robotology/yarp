# Copyright (C) 2017 iCub Facility, Istituto Italiano di Tecnologia
# Author: Andrea Ruzzenenti <andrea.ruzzenenti@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


yarp_prepare_plugin(SDLJoypad
                    CATEGORY device
                    TYPE yarp::dev::SDLJoypad
                    INCLUDE SDLJoypad.h
                    EXTRA_CONFIG WRAPPER=JoypadControlServer
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_USE_SDL")

if(ENABLE_SDLJoypad)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)

  include_directories(SYSTEM ${SDL_INCLUDE_DIR})
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  yarp_add_plugin(yarp_SDLJoypad SDLJoypad.cpp
                                 SDLJoypad.h)

  target_link_libraries(yarp_SDLJoypad YARP_OS
                                       YARP_sig
                                       YARP_dev
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
