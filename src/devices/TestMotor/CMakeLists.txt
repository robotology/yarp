# Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(test_motor
                    CATEGORY device
                    TYPE TestMotor
                    INCLUDE TestMotor.h
                    EXTRA_CONFIG WRAPPER=controlboard
                    DEFAULT ON)

if(NOT SKIP_test_motor)
  yarp_add_plugin(yarp_test_motor)

  target_sources(yarp_test_motor PRIVATE TestMotor.cpp
                                         TestMotor.h)

  target_link_libraries(yarp_test_motor PRIVATE YARP::YARP_os
                                                YARP::YARP_sig
                                                YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  yarp_install(TARGETS yarp_test_motor
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_test_motor PROPERTY FOLDER "Plugins/Device/Fake")
endif()
