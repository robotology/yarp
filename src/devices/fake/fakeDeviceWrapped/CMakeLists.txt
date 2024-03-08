# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeDeviceWrapped ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeDeviceWrapped
  CATEGORY device
  TYPE FakeDeviceWrapped
  INCLUDE FakeDeviceWrapped.h
  EXTRA_CONFIG
    WRAPPER=fakeDeviceWrapper
)

if(NOT SKIP_fakeDeviceWrapped)
  yarp_add_plugin(yarp_fakeDeviceWrapped)

  target_sources(yarp_fakeDeviceWrapped
    PRIVATE
      FakeDeviceWrapped.cpp
      FakeDeviceWrapped.h
  )

  target_link_libraries(yarp_fakeDeviceWrapped
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
    TARGETS yarp_fakeDeviceWrapped
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeDeviceWrapped PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
