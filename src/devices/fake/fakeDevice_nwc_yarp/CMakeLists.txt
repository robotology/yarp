# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fake_nwc_yarp ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeDevice_nwc_yarp
  CATEGORY device
  TYPE FakeDevice_nwc_yarp
  INCLUDE fakeDevice_nwc_yarp.h
  DEFAULT ON
)

if(NOT SKIP_fakeDevice_nwc_yarp)
  yarp_add_plugin(fakeDevice_nwc_yarp)

  target_sources(fakeDevice_nwc_yarp
    PRIVATE
    fakeDevice_nwc_yarp.cpp
    fakeDevice_nwc_yarp.h
  )

  target_sources(fakeDevice_nwc_yarp PRIVATE $<TARGET_OBJECTS:fakeTestMsgs>)

  target_include_directories(fakeDevice_nwc_yarp PRIVATE
    $<TARGET_PROPERTY:fakeTestMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(fakeDevice_nwc_yarp PRIVATE
    YARP::YARP_os
    YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  yarp_install(
    TARGETS fakeDevice_nwc_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET fakeDevice_nwc_yarp PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
