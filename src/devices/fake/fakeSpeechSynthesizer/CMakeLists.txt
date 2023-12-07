# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeSpeechSynthesizer ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeSpeechSynthesizer
  CATEGORY device
  TYPE FakeSpeechSynthesizer
  INCLUDE fakeSpeechSynthesizer.h
)

if(ENABLE_fakeSpeechSynthesizer)
  yarp_add_plugin(yarp_fakeSpeechSynthesizer)

  target_sources(yarp_fakeSpeechSynthesizer
    PRIVATE
      fakeSpeechSynthesizer.cpp
      fakeSpeechSynthesizer.h
  )

  target_link_libraries(yarp_fakeSpeechSynthesizer
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_fakeSpeechSynthesizer
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeSpeechSynthesizer PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
