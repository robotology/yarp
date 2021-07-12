# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(frameTransformStorage
                    CATEGORY device
                    TYPE FrameTransformStorage
                    INCLUDE FrameTransformStorage.h
                    DEPENDS "TARGET YARP::YARP_math"
                    DEFAULT ON)

if(NOT SKIP_frameTransformStorage)
  yarp_add_plugin(yarp_frameTransformStorage)

  target_sources(yarp_frameTransformStorage PRIVATE FrameTransformStorage.cpp
                                                    FrameTransformStorage.h)

  target_sources(yarp_frameTransformStorage PRIVATE $<TARGET_OBJECTS:FrameTransformUtils>)
  target_include_directories(yarp_frameTransformStorage PRIVATE $<TARGET_PROPERTY:FrameTransformUtils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformStorage PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev
                                                     YARP::YARP_math)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math)

  yarp_install(TARGETS yarp_frameTransformStorage
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformStorage PROPERTY FOLDER "Plugins/Device")
endif()
