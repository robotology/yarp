# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(fakeMotor
  CATEGORY device
  TYPE FakeMotor
  INCLUDE FakeMotor.h
  EXTRA_CONFIG
    WRAPPER=controlboardwrapper2
  DEFAULT OFF
  DEPENDS "NOT YARP_NO_DEPRECATED" # DEPRECATED Since YARP 3.5
)

# As discussed in #2308 test_motor should be renamed fakeMotor
#
# For now, we just add a warning, but in one of the next releases,
# test_motor will become also a DeprecatedDevice, therefore it will not be
# possible to launch it without --allow-deprecated-devices
yarp_prepare_plugin(test_motor
  CATEGORY device
  TYPE TestMotor
  INCLUDE FakeMotor.h
  EXTRA_CONFIG
    WRAPPER=controlboardwrapper2
  DEFAULT OFF
  DEPENDS "NOT YARP_NO_DEPRECATED" # DEPRECATED Since YARP 3.5
)

if(NOT SKIP_fakeMotor)
  yarp_add_plugin(yarp_fakeMotor)

  target_sources(yarp_fakeMotor PRIVATE FakeMotor.cpp
                                        FakeMotor.h)

  target_link_libraries(yarp_fakeMotor PRIVATE
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
    TARGETS yarp_fakeMotor
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeMotor PROPERTY FOLDER "Unsupported")
endif()
