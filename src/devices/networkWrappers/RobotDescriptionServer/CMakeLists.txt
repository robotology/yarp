# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(robotDescriptionServer
  CATEGORY device
  TYPE RobotDescriptionServer
  INCLUDE RobotDescriptionServer.h
  GENERATE_PARSER
  DEFAULT ON
)

if(NOT SKIP_robotDescriptionServer)
  yarp_add_plugin(yarp_robotDescriptionServer)

  target_sources(yarp_robotDescriptionServer
    PRIVATE
      RobotDescriptionServer.cpp
      RobotDescriptionServer.h
      RobotDescriptionStorage.cpp
      RobotDescriptionStorage.h
      RobotDescriptionServer_ParamsParser.cpp
      RobotDescriptionServer_ParamsParser.h
  )

  target_link_libraries(yarp_robotDescriptionServer
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
  target_sources(yarp_robotDescriptionServer PRIVATE $<TARGET_OBJECTS:IRobotDescriptionMsgs>)
  target_include_directories(yarp_robotDescriptionServer PRIVATE $<TARGET_PROPERTY:IRobotDescriptionMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  yarp_install(
    TARGETS yarp_robotDescriptionServer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_robotDescriptionServer PROPERTY FOLDER "Plugins/Device")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
