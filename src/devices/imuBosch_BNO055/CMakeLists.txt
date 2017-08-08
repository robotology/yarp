# Copyright (C) 2016 Istituto Italiano di Tecnologia (IIT)
# Authors: Alberto Cardellino
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_prepare_plugin(imuBosch_BNO055
                    CATEGORY device
                    TYPE yarp::dev::BoschIMU
                    INCLUDE imuBosch_BNO055.h
                    EXTRA_CONFIG WRAPPER=inertial
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;NOT WIN32;CREATE_LIB_MATH")

if(ENABLE_imuBosch_BNO055)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(imuBosch_BNO055 imuBosch_BNO055.cpp
                                  imuBosch_BNO055.h)
  target_link_libraries(imuBosch_BNO055 YARP::YARP_OS
                                        YARP::YARP_sig
                                        YARP::YARP_dev
                                        YARP::YARP_math)
  yarp_install(TARGETS imuBosch_BNO055
               EXPORT YARP
               COMPONENT Runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES imuBosch_BNO055.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET imuBosch_BNO055 PROPERTY FOLDER "Plugins/Device")
endif()
