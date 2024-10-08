# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(map2DStorage
  CATEGORY device
  TYPE Map2DStorage
  INCLUDE Map2DStorage.h
  GENERATE_PARSER
  EXTRA_CONFIG
    WRAPPER=map2D_nws_yarp
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_map2DStorage)
  yarp_add_plugin(yarp_map2DStorage)

  target_sources(yarp_map2DStorage
    PRIVATE
      Map2DStorage.cpp
      Map2DStorage.h
      Map2DStorage_ParamsParser.cpp
      Map2DStorage_ParamsParser.h
  )

  target_link_libraries(yarp_map2DStorage
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
    TARGETS yarp_map2DStorage
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_map2DStorage PROPERTY FOLDER "Plugins/Device")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
