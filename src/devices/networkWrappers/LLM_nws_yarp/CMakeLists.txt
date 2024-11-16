# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(LLM_nws_yarp
  CATEGORY device
  TYPE LLM_nws_yarp
  INCLUDE LLM_nws_yarp.h
  EXTRA_CONFIG
    WRAPPER=LLM_nws_yarp
  DEFAULT ON
  GENERATE_PARSER
)

if(NOT SKIP_LLM_nws_yarp)
  yarp_add_plugin(yarp_LLM_nws_yarp)

  target_sources(yarp_LLM_nws_yarp
    PRIVATE
      LLM_nws_yarp.h
      LLM_nws_yarp.cpp
      ILLMServerImpl.cpp
      ILLMServerImpl.h
      LLM_nws_yarp_ParamsParser.h
      LLM_nws_yarp_ParamsParser.cpp
  )

  target_sources(yarp_LLM_nws_yarp PRIVATE $<TARGET_OBJECTS:ILLMMsgs>)
  target_include_directories(yarp_LLM_nws_yarp
    PRIVATE
    $<TARGET_PROPERTY:ILLMMsgs,INTERFACE_INCLUDE_DIRECTORIES>
    ${CMAKE_CURRENT_SOURCE_DIR})

  target_link_libraries(yarp_LLM_nws_yarp
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
    TARGETS yarp_LLM_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_LLM_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
