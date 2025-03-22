# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(frameTransformGetMultiplexer
  CATEGORY device
  TYPE FrameTransformGetMultiplexer
  INCLUDE FrameTransformGetMultiplexer.h
  DEPENDS "TARGET YARP::YARP_math"
  EXTRA_CONFIG
    WRAPPER=frameTransformGet_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_frameTransformGetMultiplexer)
  yarp_add_plugin(yarp_frameTransformGetMultiplexer)

  target_sources(yarp_frameTransformGetMultiplexer
    PRIVATE
      FrameTransformGetMultiplexer.cpp
      FrameTransformGetMultiplexer.h
  )

  target_link_libraries(yarp_frameTransformGetMultiplexer
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_frameTransformGetMultiplexer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformGetMultiplexer PROPERTY FOLDER "Plugins/Device")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
