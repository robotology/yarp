# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(speechTranscription_nws_yarp
  CATEGORY device
  TYPE SpeechTranscription_nws_yarp
  INCLUDE SpeechTranscription_nws_yarp.h
  DEFAULT ON
  GENERATE_PARSER
)

if(NOT SKIP_speechTranscription_nws_yarp)
  yarp_add_plugin(yarp_speechTranscription_nws_yarp)

  target_sources(yarp_speechTranscription_nws_yarp
    PRIVATE
      SpeechTranscription_nws_yarp.cpp
      SpeechTranscription_nws_yarp.h
      SpeechTranscription_nws_yarp_ParamsParser.h
      SpeechTranscription_nws_yarp_ParamsParser.cpp
  )

  target_link_libraries(yarp_speechTranscription_nws_yarp
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

  target_sources(yarp_speechTranscription_nws_yarp PRIVATE $<TARGET_OBJECTS:ISpeechTranscriptionMsgs>)
  target_include_directories(yarp_speechTranscription_nws_yarp PRIVATE $<TARGET_PROPERTY:ISpeechTranscriptionMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  yarp_install(
    TARGETS yarp_speechTranscription_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_speechTranscription_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
