# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(rangefinder2D_nwc_yarp
  CATEGORY device
  TYPE Rangefinder2D_nwc_yarp
  INCLUDE Rangefinder2D_nwc_yarp.h
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
  GENERATE_PARSER
)

if(NOT SKIP_rangefinder2D_nwc_yarp)
  yarp_add_plugin(yarp_rangefinder2D_nwc_yarp)

  target_sources(yarp_rangefinder2D_nwc_yarp
    PRIVATE
      Rangefinder2D_nwc_yarp.cpp
      Rangefinder2D_nwc_yarp.h
      Rangefinder2D_nwc_yarp_ParamsParser.cpp
      Rangefinder2D_nwc_yarp_ParamsParser.h
  )

  target_sources(yarp_rangefinder2D_nwc_yarp PRIVATE $<TARGET_OBJECTS:IRangefinder2DMsgs>)
  target_include_directories(yarp_rangefinder2D_nwc_yarp PRIVATE $<TARGET_PROPERTY:IRangefinder2DMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_rangefinder2D_nwc_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_math
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_math
  )

  yarp_install(
    TARGETS yarp_rangefinder2D_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_rangefinder2D_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
