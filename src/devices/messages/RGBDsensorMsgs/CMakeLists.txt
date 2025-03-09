# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(RGBDSENSOR_THRIFT RGBDSensorMsgs.thrift)

add_library(RGBDSensorMsgs OBJECT)

target_sources(RGBDSensorMsgs PRIVATE ${RGBDSENSOR_THRIFT_GEN_FILES})
target_sources(RGBDSensorMsgs PRIVATE 
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//IDepthVisualParamsMsgs.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//IFrameGrabberControlMsgs.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//IRGBDMsgs.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//IRGBVisualParamsMsgs.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getActive.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getCameraDescription.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthAccuracy.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthClipPlanes.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthFOV.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthHeight.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthImage.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthIntrinsicParam.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthMirroring.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthResolution.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getDepthWidth.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getExtrinsic.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getFeature1.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getFeature2.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getImages.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getLastErrorMsg.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getMode.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getRgbFOV.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getRgbHeight.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getRgbImage.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getRgbIntrinsicParam.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getRgbMirroring.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getRgbResolution.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getRgbSupportedCfg.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getRgbWidth.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_getSensorStatus.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_hasAuto.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_hasFeature.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_hasManual.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_hasOnePush.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//return_hasOnOff.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//RGBDSensorMsgs.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/RGBDsensorMsgs//yCameraDescriptor.cpp
)
message(WARNING ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Oggetti in RGBDSensorMsgs: ${RGBDSENSOR_THRIFT_GEN_FILES}")
get_target_property(SOURCES_LIST RGBDSensorMsgs SOURCES)
message(WARNING "File sorgenti di RGBDSensorMsgs: ${SOURCES_LIST}")

target_link_libraries(RGBDSensorMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(RGBDSensorMsgs PUBLIC ${RGBDSENSOR_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS})

set_property(TARGET RGBDSensorMsgs PROPERTY FOLDER "Devices/Shared")
