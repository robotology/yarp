# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(remote_controlboard
  CATEGORY device
  TYPE RemoteControlBoard
  INCLUDE RemoteControlBoard.h
  EXTRA_CONFIG
    WRAPPER=controlBoard_nws_yarp
  DEFAULT ON
  GENERATE_PARSER
)

if(NOT SKIP_remote_controlboard)
  yarp_add_plugin(yarp_remote_controlboard)

  target_sources(yarp_remote_controlboard
    PRIVATE
      RemoteControlBoard.cpp
      RemoteControlBoard.h
      RemoteControlBoard_ParamsParser.cpp
      RemoteControlBoard_ParamsParser.h
      RemoteControlBoardLogComponent.cpp
      RemoteControlBoardLogComponent.h
      stateExtendedReader.cpp
      stateExtendedReader.h
  )

  target_sources(yarp_remote_controlboard PRIVATE $<TARGET_OBJECTS:ControlBoardMsgs>)
  target_include_directories(yarp_remote_controlboard PRIVATE $<TARGET_PROPERTY:ControlBoardMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_remote_controlboard
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
    TARGETS yarp_remote_controlboard
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_remote_controlboard PROPERTY FOLDER "Plugins/Device")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
