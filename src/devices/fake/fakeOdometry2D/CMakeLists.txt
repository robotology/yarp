# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeOdometry2D ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeOdometry2D
  CATEGORY device
  TYPE FakeOdometry2D
  INCLUDE FakeOdometry2D.h
  DEPENDS "TARGET YARP::YARP_math"
)

if(NOT SKIP_fakeOdometry2D)
  yarp_add_plugin(yarp_fakeOdometry2D)
  generateDeviceParamsParser(FakeOdometry2D)

  target_sources(yarp_fakeOdometry2D
    PRIVATE
          FakeOdometry2D.h
          FakeOdometry2D.cpp
          FakeOdometry2D_ParamsParser.h
          FakeOdometry2D_ParamsParser.cpp
    )

  target_link_libraries(yarp_fakeOdometry2D
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_math
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_math
  )

  yarp_install(
    TARGETS yarp_fakeOdometry2D
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeOdometry2D PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
