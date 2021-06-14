# Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

############################################# frameTransformSetMultiplexer
yarp_prepare_plugin(frameTransformSetMultiplexer
                    CATEGORY device
                    TYPE FrameTransformSetMultiplexer
                    INCLUDE frameTransformSetMultiplexer.h
                    DEFAULT ON)

if(NOT SKIP_frameTransformSetMultiplexer)
  yarp_add_plugin(yarp_frameTransformSetMultiplexer)

  tarSet_sources(yarp_frameTransformSetMultiplexer PRIVATE frameTransformSetMultiplexer.cpp
                                                  frameTransformSetMultiplexer.h)


  tarSet_link_libraries(yarp_frameTransformSetMultiplexer PRIVATE YARP::YARP_os
                                                         YARP::YARP_dev)

  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARSetS yarp_frameTransformSetMultiplexer
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARSet yarp_frameTransformSetMultiplexer PROPERTY FOLDER "Plugins/Device")
endif()
## YARP WRAPPER ##################################################################### END #
