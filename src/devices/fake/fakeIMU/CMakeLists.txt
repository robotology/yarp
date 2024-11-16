# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeIMU ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeIMU
  CATEGORY device
  TYPE FakeIMU
  INCLUDE FakeIMU.h
  GENERATE_PARSER
  EXTRA_CONFIG
    WRAPPER=inertial
  DEPENDS "TARGET YARP::YARP_math"
)

if(NOT SKIP_fakeIMU)
  yarp_add_plugin(yarp_fakeIMU)

  target_sources(yarp_fakeIMU
    PRIVATE
      FakeIMU.cpp
      FakeIMU.h
      FakeIMU_ParamsParser.cpp
      FakeIMU_ParamsParser.h
    )

  target_link_libraries(yarp_fakeIMU
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
    TARGETS yarp_fakeIMU
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeIMU PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
