# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(localization2DServer
                    CATEGORY device
                    TYPE Localization2DServer
                    INCLUDE Localization2DServer.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_localization2DServer)
  yarp_add_plugin(yarp_localization2DServer)

  target_sources(yarp_localization2DServer PRIVATE Localization2DServer.h
                                                   Localization2DServer.cpp)

  target_link_libraries(yarp_localization2DServer YARP::YARP_os
                                                  YARP::YARP_sig
                                                  YARP::YARP_dev
                                                  YARP::YARP_rosmsg
                                                  YARP::YARP_math)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg
                                                      YARP_math)

  yarp_install(TARGETS yarp_localization2DServer
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_localization2DServer PROPERTY FOLDER "Plugins/Device")
endif()
