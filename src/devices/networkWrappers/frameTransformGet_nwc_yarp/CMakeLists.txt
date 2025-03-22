# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(frameTransformGet_nwc_yarp
  CATEGORY device
  TYPE FrameTransformGet_nwc_yarp
  INCLUDE FrameTransformGet_nwc_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  EXTRA_CONFIG
    WRAPPER=frameTransformGet_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_frameTransformGet_nwc_yarp)
  yarp_add_plugin(yarp_frameTransformGet_nwc_yarp)

  target_sources(yarp_frameTransformGet_nwc_yarp
    PRIVATE
      FrameTransformGet_nwc_yarp.cpp
      FrameTransformGet_nwc_yarp.h
  )

  target_sources(yarp_frameTransformGet_nwc_yarp PRIVATE $<TARGET_OBJECTS:frameTransformStorageMsgs>)
  target_include_directories(yarp_frameTransformGet_nwc_yarp PRIVATE $<TARGET_PROPERTY:frameTransformStorageMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_frameTransformGet_nwc_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_frameTransformGet_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_frameTransformGet_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")

   if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
