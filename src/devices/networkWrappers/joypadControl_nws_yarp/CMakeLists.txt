# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(JoypadControl_nws_yarp
  CATEGORY device
  TYPE JoypadControl_nws_yarp
  INCLUDE JoypadControl_nws_yarp.h
  EXTRA_CONFIG
    WRAPPER=JoypadControl_nws_yarp
  DEFAULT ON
  GENERATE_PARSER
)

if(NOT SKIP_JoypadControl_nws_yarp)
  yarp_add_plugin(yarp_JoypadControl_nws_yarp)

  target_sources(yarp_JoypadControl_nws_yarp
    PRIVATE
      JoypadControl_nws_yarp.cpp
      JoypadControl_nws_yarp.h
      JoypadControl_nws_yarp_ParamsParser.cpp
      JoypadControl_nws_yarp_ParamsParser.h
  )

  target_sources(yarp_JoypadControl_nws_yarp PRIVATE $<TARGET_OBJECTS:joypadcontrolnetutils>)
  target_include_directories(yarp_JoypadControl_nws_yarp PRIVATE $<TARGET_PROPERTY:joypadcontrolnetutils,INTERFACE_INCLUDE_DIRECTORIES>)

  target_sources(yarp_JoypadControl_nws_yarp PRIVATE $<TARGET_OBJECTS:IJoypadControlMsgs>)
  target_include_directories(yarp_JoypadControl_nws_yarp PRIVATE $<TARGET_PROPERTY:IJoypadControlMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_JoypadControl_nws_yarp
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
    TARGETS yarp_JoypadControl_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

  set_property(TARGET yarp_JoypadControl_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")
endif()
