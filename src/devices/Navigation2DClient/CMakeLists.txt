# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(Navigation2DClient
                    CATEGORY device
                    TYPE yarp::dev::Navigation2DClient
                    INCLUDE Navigation2DClient.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_MATH_LIB"
                    DEFAULT ON)

if(NOT SKIP_Navigation2DClient)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_Navigation2DClient Navigation2DClient.cpp
                                          Navigation2DClient.h)
  target_link_libraries(yarp_Navigation2DClient YARP::YARP_OS
                                                YARP::YARP_sig
                                                YARP::YARP_dev
                                                YARP::YARP_math)
  yarp_install(TARGETS yarp_Navigation2DClient
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES Navigation2DClient.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_Navigation2DClient PROPERTY FOLDER "Plugins/Device")
endif()
