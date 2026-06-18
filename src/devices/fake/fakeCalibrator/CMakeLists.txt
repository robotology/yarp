# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeCalibrator ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeCalibrator
  CATEGORY device
  TYPE FakeCalibrator
  INCLUDE fakeCalibrator.h
  GENERATE_PARSER
  DEPENDS "TARGET YARP::YARP_math")

if(NOT SKIP_fakeCalibrator)
  yarp_add_plugin(yarp_fakeCalibrator)

  target_sources(yarp_fakeCalibrator
    PRIVATE
      FakeCalibrator.h
      FakeCalibrator.cpp
      FakeCalibrator_ParamsParser.h
      FakeCalibrator_ParamsParser.cpp
  )

  target_link_libraries(yarp_fakeCalibrator
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
    TARGETS yarp_fakeCalibrator
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeCalibrator PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
