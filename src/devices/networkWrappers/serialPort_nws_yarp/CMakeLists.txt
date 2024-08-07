# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(serialPort_nws_yarp
  CATEGORY device
  TYPE SerialPort_nws_yarp
  INCLUDE SerialPort_nws_yarp.h
  EXTRA_CONFIG
    WRAPPER=serialPort_nws_yarp
  DEFAULT ON
  GENERATE_PARSER
)

if(NOT SKIP_serialPort_nws_yarp)
  yarp_add_plugin(yarp_serialPort_nws_yarp)

  target_sources(yarp_serialPort_nws_yarp
    PRIVATE
      SerialPort_nws_yarp.cpp
      SerialPort_nws_yarp.h
      SerialPort_nws_yarp_ParamsParser.cpp
      SerialPort_nws_yarp_ParamsParser.h
  )

  target_link_libraries(yarp_serialPort_nws_yarp
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

  target_sources(yarp_serialPort_nws_yarp PRIVATE $<TARGET_OBJECTS:ISerialMsgs>)
  target_include_directories(yarp_serialPort_nws_yarp PRIVATE $<TARGET_PROPERTY:ISerialMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  yarp_install(
    TARGETS yarp_serialPort_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_serialPort_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
