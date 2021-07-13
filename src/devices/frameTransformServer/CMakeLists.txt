# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(CMakeRC)

cmrc_add_resource_library(frameTransformServerRC
                          TYPE OBJECT
                          WHENCE robotinterface_xml
                          PREFIX config_xml
                          robotinterface_xml/fts_full_ros.xml
                          robotinterface_xml/fts_yarp_only.xml)

yarp_prepare_plugin(frameTransformServer
  CATEGORY device
  TYPE FrameTransformServer
  INCLUDE FrameTransformServer.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_frameTransformServer)
  yarp_add_plugin(yarp_frameTransformServer)

  target_sources(yarp_frameTransformServer
    PRIVATE
      FrameTransformServer.cpp
      FrameTransformServer.h
  )

  target_link_libraries(yarp_frameTransformServer
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_math
      YARP::YARP_robotinterface
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_math
    YARP_robotinterface
  )

  target_sources(yarp_frameTransformServer PRIVATE $<TARGET_OBJECTS:frameTransformServerRC>)
  target_include_directories(yarp_frameTransformServer PRIVATE $<TARGET_PROPERTY:frameTransformServerRC,INTERFACE_INCLUDE_DIRECTORIES>)

  yarp_install(
    TARGETS yarp_frameTransformServer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformServer PROPERTY FOLDER "Plugins/Device")
endif()
