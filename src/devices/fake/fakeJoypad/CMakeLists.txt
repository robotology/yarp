# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeJoypad ON CACHE BOOL "" FORCE)
endif()
yarp_prepare_plugin(fakeJoypad
  CATEGORY device
  TYPE FakeJoypad
  INCLUDE FakeJoypad.h
  GENERATE_PARSER
  EXTRA_CONFIG
    WRAPPER=JoypadControlServer)

if(ENABLE_fakeJoypad)
  yarp_add_plugin(yarp_fakeJoypad)

  target_sources(yarp_fakeJoypad
    PRIVATE
      FakeJoypad.cpp
      FakeJoypad.h
      FakeJoypad_ParamsParser.cpp
      FakeJoypad_ParamsParser.h
  )

  target_link_libraries(yarp_fakeJoypad
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
    TARGETS yarp_fakeJoypad
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeJoypad PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
