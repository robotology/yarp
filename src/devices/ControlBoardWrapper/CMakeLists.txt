# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(
  controlboardwrapper2
  CATEGORY device
  TYPE ControlBoardWrapper
  INCLUDE ControlBoardWrapper.h
  EXTRA_CONFIG WRAPPER=controlboardwrapper2
  DEFAULT ON
)

if(NOT SKIP_controlboardwrapper2)
  yarp_add_plugin(yarp_controlboardwrapper2)

  target_compile_definitions(yarp_controlboardwrapper2
    PRIVATE
      LOG_COMPONENT="yarp.devices.controlboardwrapper2"
  )

  target_sources(yarp_controlboardwrapper2
    PRIVATE
      ControlBoardWrapper.cpp
      ControlBoardWrapper.h
      ControlBoardWrapperAmplifierControl.cpp
      ControlBoardWrapperAmplifierControl.h
      ControlBoardWrapperAxisInfo.cpp
      ControlBoardWrapperAxisInfo.h
      ControlBoardWrapperCommon.cpp
      ControlBoardWrapperCommon.h
      ControlBoardWrapperControlCalibration.cpp
      ControlBoardWrapperControlCalibration.h
      ControlBoardWrapperControlLimits.cpp
      ControlBoardWrapperControlLimits.h
      ControlBoardWrapperControlMode.cpp
      ControlBoardWrapperControlMode.h
      ControlBoardWrapperCurrentControl.cpp
      ControlBoardWrapperCurrentControl.h
      ControlBoardWrapperEncodersTimed.cpp
      ControlBoardWrapperEncodersTimed.h
      ControlBoardWrapperImpedanceControl.cpp
      ControlBoardWrapperImpedanceControl.h
      ControlBoardWrapperInteractionMode.cpp
      ControlBoardWrapperInteractionMode.h
      ControlBoardLogComponent.cpp
      ControlBoardLogComponent.h
      ControlBoardWrapperMotor.cpp
      ControlBoardWrapperMotor.h
      ControlBoardWrapperMotorEncoders.cpp
      ControlBoardWrapperMotorEncoders.h
      ControlBoardWrapperPWMControl.cpp
      ControlBoardWrapperPWMControl.h
      ControlBoardWrapperPidControl.cpp
      ControlBoardWrapperPidControl.h
      ControlBoardWrapperPositionControl.cpp
      ControlBoardWrapperPositionControl.h
      ControlBoardWrapperPositionDirect.cpp
      ControlBoardWrapperPositionDirect.h
      ControlBoardWrapperPreciselyTimed.cpp
      ControlBoardWrapperPreciselyTimed.h
      ControlBoardWrapperRemoteCalibrator.cpp
      ControlBoardWrapperRemoteCalibrator.h
      ControlBoardWrapperRemoteVariables.cpp
      ControlBoardWrapperRemoteVariables.h
      ControlBoardWrapperTorqueControl.cpp
      ControlBoardWrapperTorqueControl.h
      ControlBoardWrapperVelocityControl.cpp
      ControlBoardWrapperVelocityControl.h
      MultiJointData.h
      RPCMessagesParser.cpp
      RPCMessagesParser.h
      StreamingMessagesParser.cpp
      StreamingMessagesParser.h
      SubDevice.cpp
      SubDevice.h
  )

  target_link_libraries(yarp_controlboardwrapper2
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_rosmsg
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_rosmsg
  )

  yarp_install(
    TARGETS yarp_controlboardwrapper2
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_controlboardwrapper2 PROPERTY FOLDER "Plugins/Device")
endif()


yarp_prepare_plugin(
  controlBoard_nws_yarp
  CATEGORY device
  TYPE ControlBoard_nws_yarp
  INCLUDE ControlBoard_nws_yarp.h
  EXTRA_CONFIG WRAPPER=controlBoard_nws_yarp
  DEFAULT ON
)

if(NOT SKIP_controlBoard_nws_yarp)
  yarp_add_plugin(yarp_controlBoard_nws_yarp)

  target_compile_definitions(yarp_controlBoard_nws_yarp
    PRIVATE
      LOG_COMPONENT="yarp.devices.controlBoard_nws_yarp"
  )

  target_sources(yarp_controlBoard_nws_yarp
    PRIVATE
      ControlBoard_nws_yarp.cpp
      ControlBoard_nws_yarp.h
      RPCMessagesParser.cpp
      RPCMessagesParser.h
      StreamingMessagesParser.cpp
      StreamingMessagesParser.h
      ControlBoardLogComponent.cpp
      ControlBoardLogComponent.h
  )

  target_link_libraries(yarp_controlBoard_nws_yarp
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
    TARGETS yarp_controlBoard_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_controlBoard_nws_yarp PROPERTY FOLDER "Plugins/Device")
endif()


yarp_prepare_plugin(
  controlBoard_nws_ros
  CATEGORY device
  TYPE ControlBoard_nws_ros
  INCLUDE ControlBoard_nws_ros.h
  EXTRA_CONFIG WRAPPER=controlBoard_nws_ros
  DEFAULT ON
)

if(NOT SKIP_controlBoard_nws_ros)
  yarp_add_plugin(yarp_controlBoard_nws_ros)

  target_compile_definitions(yarp_controlBoard_nws_ros
    PRIVATE
      LOG_COMPONENT="yarp.devices.controlBoard_nws_ros"
  )

  target_sources(yarp_controlBoard_nws_ros
    PRIVATE
      ControlBoard_nws_ros.cpp
      ControlBoard_nws_ros.h
      ControlBoardLogComponent.cpp
      ControlBoardLogComponent.h
  )

  target_link_libraries(yarp_controlBoard_nws_ros
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_rosmsg
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_rosmsg
  )

  yarp_install(
    TARGETS yarp_controlBoard_nws_ros
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_controlBoard_nws_ros PROPERTY FOLDER "Plugins/Device")
endif()
