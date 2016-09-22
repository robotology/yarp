# Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Alberto Cardellino
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

if(NOT WIN32 AND CREATE_LIB_MATH)
  if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(imuBosch_BNO055
                        TYPE yarp::dev::BoschIMU
                        INCLUDE imuBosch_BNO055.h)
    yarp_install(FILES imuBosch_BNO055.ini
                 COMPONENT runtime
                 DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  endif()

  if(ENABLE_imuBosch_BNO055)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
    get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
    get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
    get_property(YARP_math_INCLUDE_DIRS TARGET YARP_math PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_OS_INCLUDE_DIRS}
                        ${YARP_sig_INCLUDE_DIRS}
                        ${YARP_dev_INCLUDE_DIRS}
                        ${YARP_math_INCLUDE_DIRS})
    yarp_add_plugin(imuBosch_BNO055 imuBosch_BNO055.cpp
                                    imuBosch_BNO055.h)
    target_link_libraries(imuBosch_BNO055 YARP_OS
                                          YARP_sig
                                          YARP_dev
                                          YARP_math)
    yarp_install(TARGETS imuBosch_BNO055
                 COMPONENT Runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  endif()
endif()

