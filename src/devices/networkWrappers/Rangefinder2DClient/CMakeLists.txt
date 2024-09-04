# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(rangefinder2DTransformer
  CATEGORY device
  TYPE Rangefinder2DTransformer
  INCLUDE Rangefinder2DTransformer.h
  DEPENDS "TARGET YARP::YARP_math"
  GENERATE_PARSER
  DEFAULT ON
)

if(NOT SKIP_rangefinder2DTransformer)
  yarp_add_plugin(yarp_rangefinder2DTransformer)

  target_sources(yarp_rangefinder2DTransformer
    PRIVATE
      Rangefinder2DTransformer.cpp
      Rangefinder2DTransformer.h
      Rangefinder2DTransformer_ParamsParser.cpp
      Rangefinder2DTransformer_ParamsParser.h
  )

  target_link_libraries(yarp_rangefinder2DTransformer
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
    TARGETS yarp_rangefinder2DTransformer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_rangefinder2DTransformer PROPERTY FOLDER "Plugins/Device")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
