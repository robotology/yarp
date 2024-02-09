# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if (YARP_COMPILE_ALL_FAKE_DEVICES)
  set(ENABLE_yarpmod_fakeLaserWithMotor ON CACHE BOOL "" FORCE)
endif()

yarp_prepare_plugin(fakeLaserWithMotor
  CATEGORY device
  TYPE FakeLaserWithMotor
  INCLUDE FakeLaserWithMotor.h
  DEPENDS "TARGET YARP::YARP_math")

if(NOT SKIP_fakeLaserWithMotor)
  yarp_add_plugin(yarp_fakeLaserWithMotor)
  generateDeviceParamsParser(FakeLaserWithMotor)

  target_sources(yarp_fakeLaserWithMotor
    PRIVATE
      FakeLaserWithMotor.h
      FakeLaserWithMotor.cpp
      FakeLaserWithMotor_laser.cpp
      FakeLaserWithMotor_motors.cpp
      FakeLaserWithMotor_utils.cpp
      FakeLaserWithMotor_ParamsParser.cpp
      FakeLaserWithMotor_ParamsParser.h
  )

  target_link_libraries(yarp_fakeLaserWithMotor
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
    TARGETS yarp_fakeLaserWithMotor
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeLaserWithMotor PROPERTY FOLDER "Plugins/Device/Fake")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
