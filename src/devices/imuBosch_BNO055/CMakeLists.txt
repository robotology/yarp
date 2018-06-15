# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(imuBosch_BNO055
                    CATEGORY device
                    TYPE yarp::dev::BoschIMU
                    INCLUDE imuBosch_BNO055.h
                    EXTRA_CONFIG WRAPPER=inertial
                    DEPENDS "NOT WIN32;YARP_HAS_MATH_LIB")

if(ENABLE_imuBosch_BNO055)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_imuBosch_BNO055 imuBosch_BNO055.cpp
                                       imuBosch_BNO055.h)
  target_link_libraries(yarp_imuBosch_BNO055 PRIVATE YARP::YARP_OS
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev
                                                     YARP::YARP_math)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math)

  yarp_install(TARGETS yarp_imuBosch_BNO055
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT Runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES imuBosch_BNO055.ini
               COMPONENT ${YARP_PLUGIN_MASTER}
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_imuBosch_BNO055 PROPERTY FOLDER "Plugins/Device")
endif()
