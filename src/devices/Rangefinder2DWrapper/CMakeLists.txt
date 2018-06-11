# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(Rangefinder2DWrapper
                    CATEGORY device
                    TYPE yarp::dev::Rangefinder2DWrapper
                    INCLUDE Rangefinder2DWrapper.h
                    DEFAULT ON)

if(NOT SKIP_Rangefinder2DWrapper)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_Rangefinder2DWrapper Rangefinder2DWrapper.cpp
                                            Rangefinder2DWrapper.h)
  target_link_libraries(yarp_Rangefinder2DWrapper YARP::YARP_OS
                                                  YARP::YARP_sig
                                                  YARP::YARP_dev
                                                  YARP::YARP_rosmsg)
  yarp_install(TARGETS yarp_Rangefinder2DWrapper
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES Rangefinder2DWrapper.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_Rangefinder2DWrapper PROPERTY FOLDER "Plugins/Device")
endif()
