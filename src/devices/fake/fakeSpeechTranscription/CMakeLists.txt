# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeSpeechTranscription ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeSpeechTranscription
  CATEGORY device
  TYPE FakeSpeechTranscription
  INCLUDE FakeSpeechTranscription.h
)

if(ENABLE_fakeSpeechTranscription)
  yarp_add_plugin(yarp_fakeSpeechTranscription)
  generateDeviceParamsParser(FakeSpeechTranscription)

  target_sources(yarp_fakeSpeechTranscription
    PRIVATE
      FakeSpeechTranscription.cpp
      FakeSpeechTranscription.h
      FakeSpeechTranscription_ParamsParser.cpp
      FakeSpeechTranscription_ParamsParser.h
  )

  target_link_libraries(yarp_fakeSpeechTranscription
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_fakeSpeechTranscription
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeSpeechTranscription PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
