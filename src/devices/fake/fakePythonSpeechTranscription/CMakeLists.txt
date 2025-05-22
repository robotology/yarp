# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakePythonSpeechTranscription ON CACHE BOOL "" FORCE)
endif()

if (NOT BUILD_SHARED_LIBS)
  set(ENABLE_yarpmod_fakePythonSpeechTranscription OFF CACHE BOOL "" FORCE)
  message(WARNING "fakePythonSpeechTranscription device cannot be built as static plugin")
endif()

yarp_prepare_plugin(fakePythonSpeechTranscription
  CATEGORY device
  TYPE FakePythonSpeechTranscription
  INCLUDE FakePythonSpeechTranscription.h
  GENERATE_PARSER
)

if(ENABLE_fakePythonSpeechTranscription)
  find_package(Python REQUIRED Development)
  yarp_add_plugin(yarp_fakePythonSpeechTranscription)

  target_sources(yarp_fakePythonSpeechTranscription
    PRIVATE
      FakePythonSpeechTranscription.cpp
      FakePythonSpeechTranscription.h
      FakePythonSpeechTranscription_ParamsParser.cpp
      FakePythonSpeechTranscription_ParamsParser.h
  )

  target_link_libraries(yarp_fakePythonSpeechTranscription
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
      Python::Python
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_fakePythonSpeechTranscription
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  yarp_install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/Module.py
                DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/tests/fakePythonSpeechTranscription)

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakePythonSpeechTranscription PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
