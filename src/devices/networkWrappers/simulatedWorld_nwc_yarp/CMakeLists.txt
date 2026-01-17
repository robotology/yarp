# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(simulatedWorld_nwc_yarp
  CATEGORY device
  TYPE SimulatedWorld_nwc_yarp
  INCLUDE SimulatedWorld_nwc_yarp.h
  GENERATE_PARSER
  DEFAULT ON
)

if(NOT SKIP_simulatedWorld_nwc_yarp)
  yarp_add_plugin(yarp_simulatedWorld_nwc_yarp)

  target_sources(yarp_simulatedWorld_nwc_yarp
    PRIVATE
      SimulatedWorld_nwc_yarp.cpp
      SimulatedWorld_nwc_yarp.h
      SimulatedWorld_nwc_yarp_ParamsParser.cpp
      SimulatedWorld_nwc_yarp_ParamsParser.h
  )

  target_sources(yarp_simulatedWorld_nwc_yarp PRIVATE $<TARGET_OBJECTS:ISimulatedWorldMsgs>)
  target_include_directories(yarp_simulatedWorld_nwc_yarp PRIVATE $<TARGET_PROPERTY:ISimulatedWorldMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_simulatedWorld_nwc_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_simulatedWorld_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_simulatedWorld_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
