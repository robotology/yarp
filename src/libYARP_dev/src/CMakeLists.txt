# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

add_library(YARP_dev)
add_library(YARP::YARP_dev ALIAS YARP_dev)

set(YARP_dev_HDRS yarp/dev/all.h
                  yarp/dev/api.h
                  yarp/dev/AudioBufferSize.h
                  yarp/dev/AudioGrabberInterfaces.h
                  yarp/dev/AudioGrabberVocabs.h
                  yarp/dev/AudioPlayerDeviceBase.h
                  yarp/dev/AudioRecorderDeviceBase.h
                  yarp/dev/AudioVisualInterfaces.h
                  yarp/dev/CalibratorInterfaces.h
                  yarp/dev/CalibratorVocabs.h
                  yarp/dev/CanBusInterface.h
                  yarp/dev/CartesianControl.h
                  yarp/dev/CircularAudioBuffer.h
                  yarp/dev/ControlBoardHelper.h
                  yarp/dev/ControlBoardHelpers.h
                  yarp/dev/ControlBoardInterfaces.h
                  yarp/dev/ControlBoardInterfacesImpl.h
                  yarp/dev/ControlBoardPid.h
                  yarp/dev/ControlBoardVocabs.h
                  yarp/dev/DeviceDriver.h
                  yarp/dev/DriverLinkCreator.h
                  yarp/dev/Drivers.h
                  yarp/dev/GPUInterface.h
                  yarp/dev/GazeControl.h
                  yarp/dev/GenericVocabs.h
                  yarp/dev/IAmplifierControl.h
                  yarp/dev/IAnalogSensor.h
                  yarp/dev/IAudioGrabberSound.h
                  yarp/dev/IAudioRender.h
                  yarp/dev/IAudioVisualGrabber.h
                  yarp/dev/IAudioVisualStream.h
                  yarp/dev/IAxisInfo.h
                  yarp/dev/IBattery.h
                  yarp/dev/ICalibrator.h
                  yarp/dev/IControlCalibration.h
                  yarp/dev/IControlDebug.h
                  yarp/dev/IControlLimits.h
                  yarp/dev/IControlMode.h
                  yarp/dev/ICurrentControl.h
                  yarp/dev/IDepthVisualParams.h
                  yarp/dev/IEncoders.h
                  yarp/dev/IEncodersTimed.h
                  yarp/dev/IFrameGrabberControls.h
                  yarp/dev/IFrameGrabberControlsDC1394.h
                  yarp/dev/IFrameGrabberImage.h
                  yarp/dev/IFrameGrabberImage-inl.h
                  yarp/dev/IFrameWriterAudioVisual.h
                  yarp/dev/IFrameWriterImage.h
                  yarp/dev/IGenericSensor.h
                  yarp/dev/IHapticDevice.h
                  yarp/dev/IImpedanceControl.h
                  yarp/dev/IInteractionMode.h
                  yarp/dev/IJoypadController.h
                  yarp/dev/IMotor.h
                  yarp/dev/IMotorEncoders.h
                  yarp/dev/IMultipleWrapper.h
                  yarp/dev/IPWMControl.h
                  yarp/dev/IPidControl.h
                  yarp/dev/IPositionControl.h
                  yarp/dev/IPositionDirect.h
                  yarp/dev/IPreciselyTimed.h
                  yarp/dev/IRGBDSensor.h
                  yarp/dev/IRangefinder2D.h
                  yarp/dev/IRemoteCalibrator.h
                  yarp/dev/IRemoteVariables.h
                  yarp/dev/IRgbVisualParams.h
                  yarp/dev/IRobotDescription.h
                  yarp/dev/ISerialDevice.h
                  yarp/dev/ITorqueControl.h
                  yarp/dev/IVelocityControl.h
                  yarp/dev/IVirtualAnalogSensor.h
                  yarp/dev/IVisualServoing.h
                  yarp/dev/IWrapper.h
                  yarp/dev/ImplementAmplifierControl.h
                  yarp/dev/ImplementAxisInfo.h
                  yarp/dev/ImplementControlBoardInterfaces.h
                  yarp/dev/ImplementControlCalibration.h
                  yarp/dev/ImplementControlLimits.h
                  yarp/dev/ImplementControlMode.h
                  yarp/dev/ImplementCurrentControl.h
                  yarp/dev/ImplementEncoders.h
                  yarp/dev/ImplementEncodersTimed.h
                  yarp/dev/ImplementImpedanceControl.h
                  yarp/dev/ImplementInteractionMode.h
                  yarp/dev/ImplementMotor.h
                  yarp/dev/ImplementMotorEncoders.h
                  yarp/dev/ImplementPWMControl.h
                  yarp/dev/ImplementPidControl.h
                  yarp/dev/ImplementPositionControl.h
                  yarp/dev/ImplementPositionDirect.h
                  yarp/dev/ImplementRemoteVariables.h
                  yarp/dev/ImplementTorqueControl.h
                  yarp/dev/ImplementVelocityControl.h
                  yarp/dev/ImplementVirtualAnalogSensor.h
                  yarp/dev/LaserMeasurementData.h
                  yarp/dev/Lidar2DDeviceBase.h
                  yarp/dev/MultipleAnalogSensorsInterfaces.h
                  yarp/dev/PidEnums.h
                  yarp/dev/PolyDriver.h
                  yarp/dev/PolyDriverDescriptor.h
                  yarp/dev/PolyDriverList.h
                  yarp/dev/RGBDSensorParamParser.h
                  yarp/dev/ServiceInterfaces.h
                  yarp/dev/WrapperMultiple.h
                  yarp/dev/WrapperSingle.h)

if(TARGET YARP::YARP_math)
  list(APPEND YARP_dev_HDRS yarp/dev/IFrameTransform.h
                            yarp/dev/IFrameTransformStorage.h
                            yarp/dev/IFrameTransformClientControl.h
                            yarp/dev/ILocalization2D.h
                            yarp/dev/IMap2D.h
                            yarp/dev/INavigation2D.h
                            yarp/dev/Map2DLocation.h
                            yarp/dev/Map2DArea.h
                            yarp/dev/MapGrid2D.h
                            yarp/dev/Map2DPath.h
                            yarp/dev/NavTypes.h
                            yarp/dev/MapGrid2DInfo.h)
endif()

if(NOT YARP_NO_DEPRECATED)
  list(APPEND YARP_dev_HDRS yarp/dev/FrameGrabberControl2.h      # DEPRECATED Since YARP 3.0.0
                            yarp/dev/IControlLimits2.h           # DEPRECATED Since YARP 3.0.0
                            yarp/dev/IControlMode2.h             # DEPRECATED Since YARP 3.0.0
                            yarp/dev/IPositionControl2.h         # DEPRECATED Since YARP 3.0.0
                            yarp/dev/IVelocityControl2.h         # DEPRECATED Since YARP 3.0.0
                            yarp/dev/ImplementControlLimits2.h   # DEPRECATED Since YARP 3.0.0
                            yarp/dev/ImplementControlMode2.h     # DEPRECATED Since YARP 3.0.0
                            yarp/dev/ImplementPositionControl2.h # DEPRECATED Since YARP 3.0.0
                            yarp/dev/ImplementVelocityControl2.h # DEPRECATED Since YARP 3.0.0
                            yarp/dev/DataSource.h                # DEPRECATED Since YARP 3.3.0
                            yarp/dev/GenericSensorInterfaces.h   # DEPRECATED Since YARP 3.3.0
                            yarp/dev/PreciselyTimed.h            # DEPRECATED Since YARP 3.3.0
                            yarp/dev/SerialInterfaces.h          # DEPRECATED Since YARP 3.3.0
                            yarp/dev/Wrapper.h                   # DEPRECATED Since YARP 3.3.0
                            yarp/dev/FrameGrabberInterfaces.h    # DEPRECATED Since YARP 3.5.0
                            yarp/dev/IFrameGrabber.h             # DEPRECATED Since YARP 3.5.0
                            yarp/dev/IFrameGrabberRgb.h          # DEPRECATED Since YARP 3.5.0
                            yarp/dev/IVisualParams.h             # DEPRECATED Since YARP 3.5.0
  )
endif()

set(YARP_dev_IMPL_HDRS yarp/dev/impl/FixedSizeBuffersManager.h
                       yarp/dev/impl/FixedSizeBuffersManager-inl.h)

set(YARP_dev_SRCS yarp/dev/AudioBufferSize.cpp
                  yarp/dev/AudioPlayerDeviceBase.cpp
                  yarp/dev/AudioRecorderDeviceBase.cpp
                  yarp/dev/CanBusInterface.cpp
                  yarp/dev/CartesianControl.cpp
                  yarp/dev/ControlBoardHelper.cpp
                  yarp/dev/ControlBoardInterfacesImpl.cpp
                  yarp/dev/ControlBoardPid.cpp
                  yarp/dev/DeviceDriver.cpp
                  yarp/dev/DriverLinkCreator.cpp
                  yarp/dev/Drivers.cpp
                  yarp/dev/GazeControl.cpp
                  yarp/dev/IAnalogSensor.cpp
                  yarp/dev/IAudioGrabberSound.cpp
                  yarp/dev/IAudioRender.cpp
                  yarp/dev/IAudioVisualGrabber.cpp
                  yarp/dev/IAudioVisualStream.cpp
                  yarp/dev/IBattery.cpp
                  yarp/dev/IDepthVisualParams.cpp
                  yarp/dev/IFrameGrabberControls.cpp
                  yarp/dev/IFrameGrabberControlsDC1394.cpp
                  yarp/dev/IFrameGrabberImage.cpp
                  yarp/dev/IFrameWriterAudioVisual.cpp
                  yarp/dev/IFrameWriterImage.cpp
                  yarp/dev/IGenericSensor.cpp
                  yarp/dev/IJoypadController.cpp
                  yarp/dev/IMultipleWrapper.cpp
                  yarp/dev/IPreciselyTimed.cpp
                  yarp/dev/IRGBDSensor.cpp
                  yarp/dev/IRangefinder2D.cpp
                  yarp/dev/IRemoteCalibrator.cpp
                  yarp/dev/IRgbVisualParams.cpp
                  yarp/dev/IRobotDescription.cpp
                  yarp/dev/ISerialDevice.cpp
                  yarp/dev/IVisualServoing.cpp
                  yarp/dev/IWrapper.cpp
                  yarp/dev/ImplementAmplifierControl.cpp
                  yarp/dev/ImplementAxisInfo.cpp
                  yarp/dev/ImplementControlCalibration.cpp
                  yarp/dev/ImplementControlLimits.cpp
                  yarp/dev/ImplementControlMode.cpp
                  yarp/dev/ImplementCurrentControl.cpp
                  yarp/dev/ImplementEncoders.cpp
                  yarp/dev/ImplementEncodersTimed.cpp
                  yarp/dev/ImplementImpedanceControl.cpp
                  yarp/dev/ImplementInteractionMode.cpp
                  yarp/dev/ImplementMotor.cpp
                  yarp/dev/ImplementMotorEncoders.cpp
                  yarp/dev/ImplementPWMControl.cpp
                  yarp/dev/ImplementPidControl.cpp
                  yarp/dev/ImplementPositionControl.cpp
                  yarp/dev/ImplementPositionDirect.cpp
                  yarp/dev/ImplementRemoteVariables.cpp
                  yarp/dev/ImplementTorqueControl.cpp
                  yarp/dev/ImplementVelocityControl.cpp
                  yarp/dev/ImplementVirtualAnalogSensor.cpp
                  yarp/dev/LaserMeasurementData.cpp
                  yarp/dev/Lidar2DDeviceBase.cpp
                  yarp/dev/MultipleAnalogSensorsInterfaces.cpp
                  yarp/dev/PolyDriver.cpp
                  yarp/dev/PolyDriverDescriptor.cpp
                  yarp/dev/PolyDriverList.cpp
                  yarp/dev/RGBDSensorParamParser.cpp
                  yarp/dev/WrapperMultiple.cpp
                  yarp/dev/WrapperSingle.cpp)

if(TARGET YARP::YARP_math)
  list(APPEND YARP_dev_SRCS yarp/dev/IFrameTransform.cpp
                            yarp/dev/IFrameTransformStorage.cpp
                            yarp/dev/IFrameTransformClientControl.cpp
                            yarp/dev/IMap2D.cpp
                            yarp/dev/INavigation2D.cpp
                            yarp/dev/MapGrid2D.cpp
                            yarp/dev/Map2DLocation.cpp
                            yarp/dev/Map2DArea.cpp
                            yarp/dev/Map2DPath.cpp
                            yarp/dev/MapGrid2DInfo.cpp)
endif()

if(NOT YARP_NO_DEPRECATED)
  list(APPEND YARP_dev_SRCS yarp/dev/IFrameGrabber.cpp         # DEPRECATED Since YARP 3.5.0
                            yarp/dev/IFrameGrabberRgb.cpp      # DEPRECATED Since YARP 3.5.0

  )
endif()


# Handle the YARP thrift messages
include(YarpChooseIDL)

set(YARP_dev_IDL idl/stateExt.thrift
                 idl/audioBufferSizeData.thrift
                 idl/OdometryData.thrift
                 idl/OdometryData6D.thrift
                 idl/LaserScan2D.thrift
                 idl/MobileBaseVelocity.thrift
                 idl/audioPlayerStatus.thrift
                 idl/audioRecorderStatus.thrift)

if(TARGET YARP::YARP_math)
  list(APPEND YARP_dev_IDL idl/Map2DLocationData.thrift
                           idl/Map2DPathData.thrift)
endif()

yarp_choose_idl(YARP_dev_idl ${YARP_dev_IDL})

unset(YARP_dev_idl_HDRS)
unset(YARP_dev_idl_IMPL_HDRS)
foreach(_f IN LISTS YARP_dev_idl_GEN_FILES)
  if("${_f}" MATCHES "\\.h$")
    if ("${_f}" MATCHES "yarp/dev" AND NOT "${_f}" MATCHES "yarp/dev/impl")
      list(APPEND YARP_dev_idl_HDRS "${_f}")
    else()
      list(APPEND YARP_dev_idl_IMPL_HDRS ${_f})
    endif()
  endif()
endforeach()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_dev_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_dev_HDRS}
                   ${YARP_dev_IMPL_HDRS})
source_group(TREE "${YARP_dev_idl_BUILD_INTERFACE_INCLUDE_DIRS}"
             PREFIX "Generated Files"
             FILES ${YARP_dev_idl_GEN_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Devices Files"
             FILES ${YARP_dev_devices_SRCS}
                   ${YARP_dev_devices_HDRS})


target_sources(YARP_dev PRIVATE ${YARP_dev_SRCS}
                                ${YARP_dev_HDRS}
                                ${YARP_dev_IMPL_HDRS}
                                ${YARP_dev_devices_SRCS}
                                ${YARP_dev_devices_HDRS}
                                ${YARP_dev_idl_GEN_FILES})

target_include_directories(YARP_dev PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                           $<BUILD_INTERFACE:${YARP_dev_idl_BUILD_INTERFACE_INCLUDE_DIRS}>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_compile_features(YARP_dev PUBLIC cxx_std_14)

target_link_libraries(YARP_dev PUBLIC YARP::YARP_conf
                                      YARP::YARP_os
                                      YARP::YARP_sig
                               PRIVATE YARP::YARP_rosmsg)
list(APPEND YARP_dev_PUBLIC_DEPS YARP_conf
                                 YARP_os
                                 YARP_sig)
list(APPEND YARP_dev_PRIVATE_DEPS YARP_rosmsg)

if(TARGET YARP::YARP_math)
  target_link_libraries(YARP_dev PUBLIC YARP::YARP_math)
  list(APPEND YARP_dev_PUBLIC_DEPS YARP_math)
endif()

if(YARP_HAS_ZLIB)
  target_include_directories(YARP_dev SYSTEM PRIVATE ${ZLIB_INCLUDE_DIR})
  target_compile_definitions(YARP_dev PRIVATE YARP_HAS_ZLIB)
  target_link_libraries(YARP_dev PRIVATE ${ZLIB_LIBRARY})
  list(APPEND YARP_dev_PRIVATE_DEPS ZLIB)
endif()

set_property(TARGET YARP_dev PROPERTY PUBLIC_HEADER ${YARP_dev_HDRS}
                                                    ${YARP_dev_idl_HDRS})
set_property(TARGET YARP_dev PROPERTY PRIVATE_HEADER ${YARP_dev_IMPL_HDRS}
                                                     ${YARP_dev_idl_IMPL_HDRS})
set_property(TARGET YARP_dev PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_dev PROPERTY SOVERSION ${YARP_SOVERSION})
set_property(TARGET YARP_dev PROPERTY FOLDER "Libraries")


install(TARGETS YARP_dev
        EXPORT YARP_dev
        RUNTIME
          DESTINATION "${CMAKE_INSTALL_BINDIR}"
          COMPONENT YARP_dev
        LIBRARY
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_dev
          NAMELINK_COMPONENT YARP_dev-dev
        ARCHIVE
          DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          COMPONENT YARP_dev-dev
        PUBLIC_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dev"
          COMPONENT YARP_dev-dev
        PRIVATE_HEADER
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dev/impl"
          COMPONENT YARP_dev-priv-dev)


set(YARP_dev_PUBLIC_DEPS ${YARP_dev_PUBLIC_DEPS} PARENT_SCOPE)
set(YARP_dev_PRIVATE_DEPS ${YARP_dev_PRIVATE_DEPS} PARENT_SCOPE)
