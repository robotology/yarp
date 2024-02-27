# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeMicrophone ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeMicrophone
  CATEGORY device
  TYPE FakeMicrophone
  INCLUDE FakeMicrophone.h
  GENERATE_PARSER
  EXTRA_CONFIG
    WRAPPER=AudioRecorderWrapper
)

if(NOT SKIP_fakeMicrophone)
  yarp_add_plugin(yarp_fakeMicrophone)

  target_sources(yarp_fakeMicrophone
    PRIVATE
      FakeMicrophone.cpp
      FakeMicrophone.h
      FakeMicrophone_ParamsParser.cpp
      FakeMicrophone_ParamsParser.h
  )

  target_link_libraries(yarp_fakeMicrophone
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
    TARGETS yarp_fakeMicrophone
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeMicrophone PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
