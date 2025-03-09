# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_DEVICE_PLUGINS)
  return()
endif()

include(YarpChooseIDL)
yarp_choose_idl(FRAMEGRABBER_THRIFT frameGrabberMsgs.thrift)

add_library(FrameGrabberMsgs OBJECT)

target_sources(FrameGrabberMsgs PRIVATE ${FRAMEGRABBER_THRIFT_GEN_FILES})

target_link_libraries(FrameGrabberMsgs
  PRIVATE
    YARP::YARP_dev
    YARP::YARP_os
    YARP::YARP_sig
)

target_include_directories(FrameGrabberMsgs PUBLIC ${FRAMEGRABBER_THRIFT_BUILD_INTERFACE_INCLUDE_DIRS}
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//IFrameGrabberControlDC1394Msgs.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//IFrameGrabberControlMsgs.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//IRGBVisualParamsMsgs.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getActive.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getBytesPerPacketDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getCameraDescription.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getColorCodingDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getColorCodingMaskDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getFeature1.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getFeature2.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getFormat7MaxWindowDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getFormat7WindowDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getFPSDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getFPSMaskDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getISOSpeedDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getMode.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getOperationModeDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getRgbFOV.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getRgbHeight.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getRgbIntrinsicParam.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getRgbMirroring.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getRgbResolution.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getRgbSupportedCfg.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getRgbWidth.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getTransmissionDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getVideoModeDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_getVideoModeMaskDC1394.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_hasAuto.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_hasFeature.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_hasManual.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_hasOnePush.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//return_hasOnOff.cpp
C:/Software/iCubSoftware/yarp-master/build/src/devices/messages/frameGrabberMsgs//yCameraDescriptor.cpp
)

set_property(TARGET FrameGrabberMsgs PROPERTY FOLDER "Devices/Shared")
