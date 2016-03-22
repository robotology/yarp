# Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Alberto Cardellino
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

yarp_prepare_device(imuBosch_BNO055 TYPE yarp::dev::BoschIMU INCLUDE imuBosch_BNO055.h)

 if (ENABLE_imuBosch_BNO055)

    if(WIN32)
        message("The device IMU BOSCH is not available on WINDOWS now, please un select it")
    else()
        include_directories(${YARP_INCLUDE_DIRS} ${ACE_INCLUDE_DIRS})
        include_directories(${CMAKE_CURRENT_SOURCE_DIR})

        yarp_add_plugin(imuBosch_BNO055 imuBosch_BNO055.cpp imuBosch_BNO055.h)
        target_link_libraries(imuBosch_BNO055  ${YARP_LIBRARIES} )
        yarp_install( TARGETS imuBosch_BNO055
                        COMPONENT Runtime
                        LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR})

        yarp_install( FILES imuBosch_BNO055.ini
                        DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
    endif()
endif (ENABLE_imuBosch_BNO055)

