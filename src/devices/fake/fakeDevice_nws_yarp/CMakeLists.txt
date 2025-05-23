# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fake_nws_yarp ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeDevice_nws_yarp
  CATEGORY device
  TYPE FakeDevice_nws_yarp
  INCLUDE fakeDevice_nws_yarp.h
  DEFAULT ON
)

if(NOT SKIP_fakeDevice_nws_yarp)
  yarp_add_plugin(yarp_fakeDevice_nws_yarp)

  target_sources(yarp_fakeDevice_nws_yarp
    PRIVATE
    fakeDevice_nws_yarp.cpp
    fakeDevice_nws_yarp.h
    fakeDeviceServerImpl.cpp
    fakeDeviceServerImpl.h
  )

  target_sources(yarp_fakeDevice_nws_yarp PRIVATE $<TARGET_OBJECTS:fakeTestMsgs>)

  target_include_directories(yarp_fakeDevice_nws_yarp PRIVATE
    $<TARGET_PROPERTY:fakeTestMsgs,INTERFACE_INCLUDE_DIRECTORIES>
    ${CMAKE_CURRENT_SOURCE_DIR})

  target_link_libraries(yarp_fakeDevice_nws_yarp PRIVATE
    YARP::YARP_os
    YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_fakeDevice_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeDevice_nws_yarp PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
