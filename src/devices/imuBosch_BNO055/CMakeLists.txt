# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(imuBosch_BNO055
                    CATEGORY device
                    TYPE BoschIMU
                    INCLUDE imuBosch_BNO055.h
                    EXTRA_CONFIG WRAPPER=inertial
                    DEPENDS "NOT WIN32;TARGET YARP::YARP_math;YARP_HAS_I2C")

if(ENABLE_imuBosch_BNO055)
  yarp_add_plugin(yarp_imuBosch_BNO055)

  target_sources(yarp_imuBosch_BNO055 PRIVATE imuBosch_BNO055.cpp
                                              imuBosch_BNO055.h)

  target_link_libraries(yarp_imuBosch_BNO055 PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev
                                                     YARP::YARP_math)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math)

  target_compile_definitions(yarp_imuBosch_BNO055 PRIVATE ${I2C_DEFINITIONS})
  target_include_directories(yarp_imuBosch_BNO055 SYSTEM PRIVATE ${I2C_INCLUDE_DIRS})
  target_link_libraries(yarp_imuBosch_BNO055 PRIVATE ${I2C_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS I2C) (not using targets)

  yarp_install(TARGETS yarp_imuBosch_BNO055
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_imuBosch_BNO055 PROPERTY FOLDER "Plugins/Device")
endif()
