# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(urbtc
                    CATEGORY device
                    TYPE yarp::dev::UrbtcControl
                    INCLUDE UrbtcControl.h
                    EXTRA_CONFIG WRAPPER=controlboard
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;UNIX;YARP_COMPILE_UNMAINTAINED"
                    ADVANCED)

if(NOT SKIP_urbtc)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(urbtc UrbtcControl.cpp
                        UrbtcControl.h)
  target_link_libraries(yarp_urbtc YARP::YARP_OS
                                   YARP::YARP_dev)
  yarp_install(TARGETS urbtc
                EXPORT YARP
                COMPONENT runtime
                LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES urbtc.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET urbtc PROPERTY FOLDER "Unsupported")
endif()
