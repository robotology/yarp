# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeDepthCamera        ON CACHE BOOL "" FORCE)
  set(ENABLE_yarpmod_fakeDepthCamera_mini   ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeDepthCamera
  CATEGORY device
  TYPE FakeDepthCameraDriver
  INCLUDE FakeDepthCameraDriver.h
  GENERATE_PARSER
  EXTRA_CONFIG
    WRAPPER=RGBDSensor_nws_yarp
)

if(ENABLE_fakeDepthCamera)

  yarp_add_plugin(yarp_fakeDepthCamera)

  target_sources(yarp_fakeDepthCamera
    PRIVATE
      FakeDepthCameraDriver.cpp
      FakeDepthCameraDriver.h
      FakeDepthCameraDriver_mini.cpp
      FakeDepthCameraDriver_mini.h
      FakeDepthCameraDriver_ParamsParser.cpp
      FakeDepthCameraDriver_ParamsParser.h
  )

  target_link_libraries(yarp_fakeDepthCamera
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
    TARGETS yarp_fakeDepthCamera
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeDepthCamera PROPERTY FOLDER "Plugins/Device/Fake")

endif()

#------------------

yarp_prepare_plugin(fakeDepthCamera_mini
  CATEGORY device
  TYPE FakeDepthCameraDriver_mini
  INCLUDE FakeDepthCameraDriver_mini.h
  EXTRA_CONFIG
    WRAPPER=rgbdSensor_nws_yarp
)

if(ENABLE_fakeDepthCamera_mini)

  yarp_add_plugin(yarp_fakeDepthCamera_mini)

  target_sources(yarp_fakeDepthCamera_mini
    PRIVATE
      FakeDepthCameraDriver_mini.cpp
      FakeDepthCameraDriver_mini.h
      FakeDepthCameraDriver_ParamsParser.cpp
      FakeDepthCameraDriver_ParamsParser.h
  )

  target_link_libraries(yarp_fakeDepthCamera_mini
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
    TARGETS yarp_fakeDepthCamera_mini
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeDepthCamera_mini PROPERTY FOLDER "Plugins/Device/Fake")

endif()

if (ENABLE_fakeDepthCamera OR ENABLE_fakeDepthCamera_mini)
  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()
endif()
