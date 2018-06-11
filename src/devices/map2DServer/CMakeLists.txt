# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(map2DServer
                    CATEGORY device
                    TYPE yarp::dev::Map2DServer
                    INCLUDE Map2DServer.h
                    DEPENDS "YARP_HAS_MATH_LIB"
                    DEFAULT ON)

if(NOT SKIP_map2DServer)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_map2DServer Map2DServer.cpp
                                   Map2DServer.h)
  target_link_libraries(yarp_map2DServer YARP::YARP_OS
                                         YARP::YARP_sig
                                         YARP::YARP_dev
                                         YARP::YARP_math
                                         YARP::YARP_rosmsg)
  yarp_install(TARGETS yarp_map2DServer
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES map2DServer.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_map2DServer PROPERTY FOLDER "Plugins/Device")
endif()
