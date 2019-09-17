# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

project(YARP_dev)

set(YARP_dev_HDRS include/yarp/dev/all.h
                  include/yarp/dev/api.h
                  include/yarp/dev/AudioBufferSize.h
                  include/yarp/dev/AudioGrabberInterfaces.h
                  include/yarp/dev/AudioVisualInterfaces.h
                  include/yarp/dev/CalibratorInterfaces.h
                  include/yarp/dev/CalibratorVocabs.h
                  include/yarp/dev/CanBusInterface.h
                  include/yarp/dev/CircularAudioBuffer.h
                  include/yarp/dev/CartesianControl.h
                  include/yarp/dev/ControlBoardHelper.h
                  include/yarp/dev/ControlBoardHelpers.h
                  include/yarp/dev/ControlBoardInterfaces.h
                  include/yarp/dev/ControlBoardInterfacesImpl.h
                  include/yarp/dev/ControlBoardPid.h
                  include/yarp/dev/ControlBoardVocabs.h
                  include/yarp/dev/DataSource.h
                  include/yarp/dev/DeviceDriver.h
                  include/yarp/dev/DriverLinkCreator.h
                  include/yarp/dev/Drivers.h
                  include/yarp/dev/FrameGrabberControl2.h
                  include/yarp/dev/FrameGrabberControlImpl.h
                  include/yarp/dev/FrameGrabberInterfaces.h
                  include/yarp/dev/GazeControl.h
                  include/yarp/dev/GenericSensorInterfaces.h
                  include/yarp/dev/GenericVocabs.h
                  include/yarp/dev/GPUInterface.h
                  include/yarp/dev/IAmplifierControl.h
                  include/yarp/dev/IAnalogSensor.h
                  include/yarp/dev/IAxisInfo.h
                  include/yarp/dev/IBattery.h
                  include/yarp/dev/ICalibrator.h
                  include/yarp/dev/IControlCalibration.h
                  include/yarp/dev/IControlDebug.h
                  include/yarp/dev/IControlLimits2.h
                  include/yarp/dev/IControlLimits.h
                  include/yarp/dev/IControlMode2.h
                  include/yarp/dev/IControlMode.h
                  include/yarp/dev/ICurrentControl.h
                  include/yarp/dev/IEncoders.h
                  include/yarp/dev/IEncodersTimed.h
                  include/yarp/dev/IHapticDevice.h
                  include/yarp/dev/IImpedanceControl.h
                  include/yarp/dev/IInteractionMode.h
                  include/yarp/dev/IJoypadController.h
                  include/yarp/dev/IMotorEncoders.h
                  include/yarp/dev/IMotor.h
                  include/yarp/dev/ImplementAmplifierControl.h
                  include/yarp/dev/ImplementAxisInfo.h
                  include/yarp/dev/ImplementControlBoardInterfaces.h
                  include/yarp/dev/ImplementControlCalibration.h
                  include/yarp/dev/ImplementControlLimits2.h
                  include/yarp/dev/ImplementControlLimits.h
                  include/yarp/dev/ImplementControlMode2.h
                  include/yarp/dev/ImplementControlMode.h
                  include/yarp/dev/ImplementCurrentControl.h
                  include/yarp/dev/ImplementEncoders.h
                  include/yarp/dev/ImplementEncodersTimed.h
                  include/yarp/dev/ImplementImpedanceControl.h
                  include/yarp/dev/ImplementInteractionMode.h
                  include/yarp/dev/ImplementMotorEncoders.h
                  include/yarp/dev/ImplementMotor.h
                  include/yarp/dev/ImplementPidControl.h
                  include/yarp/dev/ImplementPositionControl2.h
                  include/yarp/dev/ImplementPositionControl.h
                  include/yarp/dev/ImplementPositionDirect.h
                  include/yarp/dev/ImplementPWMControl.h
                  include/yarp/dev/ImplementRemoteVariables.h
                  include/yarp/dev/ImplementTorqueControl.h
                  include/yarp/dev/ImplementVelocityControl2.h
                  include/yarp/dev/ImplementVelocityControl.h
                  include/yarp/dev/ImplementVirtualAnalogSensor.h
                  include/yarp/dev/IPidControl.h
                  include/yarp/dev/IPositionControl2.h
                  include/yarp/dev/IPositionControl.h
                  include/yarp/dev/IPositionDirect.h
                  include/yarp/dev/IPreciselyTimed.h
                  include/yarp/dev/IPWMControl.h
                  include/yarp/dev/IRangefinder2D.h
                  include/yarp/dev/IRemoteCalibrator.h
                  include/yarp/dev/IRemoteVariables.h
                  include/yarp/dev/IRGBDSensor.h
                  include/yarp/dev/IRobotDescription.h
                  include/yarp/dev/ITorqueControl.h
                  include/yarp/dev/IVelocityControl2.h
                  include/yarp/dev/IVelocityControl.h
                  include/yarp/dev/IVirtualAnalogSensor.h
                  include/yarp/dev/IVisualParams.h
                  include/yarp/dev/IVisualParamsImpl.h
                  include/yarp/dev/IVisualServoing.h
                  include/yarp/dev/LaserMeasurementData.h
                  include/yarp/dev/MultipleAnalogSensorsInterfaces.h
                  include/yarp/dev/PidEnums.h
                  include/yarp/dev/PolyDriverDescriptor.h
                  include/yarp/dev/PolyDriver.h
                  include/yarp/dev/PolyDriverList.h
                  include/yarp/dev/RGBDSensorParamParser.h
                  include/yarp/dev/SerialInterfaces.h
                  include/yarp/dev/ServiceInterfaces.h
                  include/yarp/dev/Wrapper.h)

if(TARGET YARP::YARP_math)
  list(APPEND YARP_dev_HDRS include/yarp/dev/IFrameTransform.h
                            include/yarp/dev/ILocalization2D.h
                            include/yarp/dev/IMap2D.h
                            include/yarp/dev/INavigation2D.h
                            include/yarp/dev/Map2DLocation.h
                            include/yarp/dev/Map2DArea.h
                            include/yarp/dev/MapGrid2D.h)
endif()

if(NOT YARP_NO_DEPRECATED) # Since YARP 3.3.0
  list(APPEND YARP_dev_HDRS include/yarp/dev/IPreciselyTimed.h)
endif()


set(YARP_dev_IMPL_HDRS include/yarp/dev/impl/FixedSizeBuffersManager.h
                       include/yarp/dev/impl/FixedSizeBuffersManager-inl.h)

set(YARP_dev_SRCS src/CanBusInterface.cpp
                  src/CartesianControl.cpp
                  src/ControlBoardInterfacesImpl.cpp
                  src/ControlBoardHelper.cpp
                  src/ControlBoardPid.cpp
                  src/DeviceDriver.cpp
                  src/Drivers.cpp
                  src/FrameGrabberControlImpl.cpp
                  src/GazeControl.cpp
                  src/IBattery.cpp
                  src/IJoypadController.cpp
                  src/IRemoteCalibrator.cpp
                  src/ImplementAmplifierControl.cpp
                  src/ImplementAxisInfo.cpp
                  src/ImplementControlCalibration.cpp
                  src/ImplementControlLimits.cpp
                  src/ImplementControlMode.cpp
                  src/ImplementCurrentControl.cpp
                  src/ImplementEncoders.cpp
                  src/ImplementEncodersTimed.cpp
                  src/ImplementImpedanceControl.cpp
                  src/ImplementInteractionMode.cpp
                  src/ImplementMotor.cpp
                  src/ImplementMotorEncoders.cpp
                  src/ImplementPidControl.cpp
                  src/ImplementPositionControl.cpp
                  src/ImplementPositionDirect.cpp
                  src/ImplementPWMControl.cpp
                  src/ImplementRemoteVariables.cpp
                  src/ImplementTorqueControl.cpp
                  src/ImplementVelocityControl.cpp
                  src/ImplementVirtualAnalogSensor.cpp
                  src/IRangefinder2D.cpp
                  src/IVisualParamsImpl.cpp
                  src/IVisualServoing.cpp
                  src/LaserMeasurementData.cpp
                  src/MultipleAnalogSensorsInterfaces.cpp
                  src/PolyDriver.cpp
                  src/PolyDriverDescriptor.cpp
                  src/PolyDriverList.cpp
                  src/PopulateDrivers.cpp
                  src/RGBDSensorParamParser.cpp)

if(TARGET YARP::YARP_math)
  list(APPEND YARP_dev_SRCS src/IFrameTransform.cpp
                            src/IMap2D.cpp
                            src/INavigation2D.cpp
                            src/MapGrid2D.cpp
                            src/Map2DArea.cpp)
endif()

set(YARP_dev_devices_SRCS src/devices/ControlBoardRemapper/ControlBoardRemapper.cpp
                          src/devices/ControlBoardRemapper/ControlBoardRemapperHelpers.cpp
                          src/devices/ControlBoardRemapper/RemoteControlBoardRemapper.cpp
                          src/devices/ControlBoardWrapper/ControlBoardWrapper.cpp
                          src/devices/ControlBoardWrapper/RPCMessagesParser.cpp
                          src/devices/ControlBoardWrapper/StreamingMessagesParser.cpp
                          src/devices/ControlBoardWrapper/SubDevice.cpp
                          src/devices/JoypadControlClient/JoypadControlClient.cpp
                          src/devices/JoypadControlServer/JoypadControlServer.cpp
                          src/devices/RobotDescriptionClient/RobotDescriptionClient.cpp
                          src/devices/RobotDescriptionServer/RobotDescriptionServer.cpp
                          src/devices/RGBDSensorClient/RGBDSensorClient.cpp
                          src/devices/RGBDSensorClient/RGBDSensorClient_StreamingMsgParser.cpp
                          src/devices/RGBDSensorWrapper/RGBDSensorWrapper.cpp
                          src/devices/ServerGrabber/ServerGrabber.cpp
                          src/devices/ServerInertial/ServerInertial.cpp
                          src/devices/VirtualAnalogWrapper/VirtualAnalogWrapper.cpp)

set(YARP_dev_devices_HDRS src/devices/ControlBoardRemapper/ControlBoardRemapper.h
                          src/devices/ControlBoardRemapper/ControlBoardRemapperHelpers.h
                          src/devices/ControlBoardRemapper/RemoteControlBoardRemapper.h
                          src/devices/ControlBoardWrapper/ControlBoardWrapper.h
                          src/devices/ControlBoardWrapper/RPCMessagesParser.h
                          src/devices/ControlBoardWrapper/StreamingMessagesParser.h
                          src/devices/ControlBoardWrapper/SubDevice.h
                          src/devices/JoypadControlClient/JoypadControlClient.h
                          src/devices/JoypadControlServer/JoypadControlNetUtils.h
                          src/devices/JoypadControlServer/JoypadControlServer.h
                          src/devices/RobotDescriptionClient/RobotDescriptionClient.h
                          src/devices/RobotDescriptionServer/RobotDescriptionServer.h
                          src/devices/RGBDSensorClient/RGBDSensorClient.h
                          src/devices/RGBDSensorClient/RGBDSensorClient_StreamingMsgParser.h
                          src/devices/RGBDSensorWrapper/RGBDSensorWrapper.h
                          src/devices/ServerGrabber/ServerGrabber.h
                          src/devices/ServerInertial/ServerInertial.h
                          src/devices/VirtualAnalogWrapper/VirtualAnalogWrapper.h)

# Handle the YARP thrift messages
include(YarpIDL)
yarp_idl_to_dir(INPUT_FILES idl/stateExt.thrift
                            idl/audioBufferSizeData.thrift
                OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code"
                SOURCES_VAR YARP_dev_idl_SRCS
                HEADERS_VAR YARP_dev_idl_all_HDRS
                INCLUDE_DIRS_VAR YARP_dev_idl_INCLUDE_DIRS)

list(APPEND YARP_dev_SRCS ${YARP_dev_idl_SRCS})
unset(YARP_dev_idl_HDRS)
foreach(_hdr IN LISTS YARP_dev_idl_all_HDRS)
  if("${_hdr}" MATCHES "yarp/dev" AND NOT "${_hdr}" MATCHES "yarp/dev/impl")
    list(APPEND YARP_dev_idl_HDRS "${_hdr}")
  else()
    list(APPEND YARP_dev_idl_IMPL_HDRS ${_hdr})
  endif()
endforeach()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Source Files"
             FILES ${YARP_dev_SRCS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Header Files"
             FILES ${YARP_dev_HDRS}
                   ${YARP_dev_IMPL_HDRS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/idl_generated_code"
             PREFIX "Generated Files"
             FILES ${YARP_dev_idl_SRCS}
                   ${YARP_dev_idl_HDRS}
                   ${YARP_dev_idl_IMPL_HDRS})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
             PREFIX "Devices Files"
             FILES ${YARP_dev_devices_SRCS}
                   ${YARP_dev_devices_HDRS})

add_library(YARP_dev ${YARP_dev_SRCS}
                     ${YARP_dev_HDRS}
                     ${YARP_dev_IMPL_HDRS}
                     ${YARP_dev_devices_SRCS}
                     ${YARP_dev_devices_HDRS})
add_library(YARP::YARP_dev ALIAS YARP_dev)

target_include_directories(YARP_dev PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                           $<BUILD_INTERFACE:${YARP_dev_idl_INCLUDE_DIRS}>
                                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_include_directories(YARP_dev PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/devices/JoypadControlServer)

target_link_libraries(YARP_dev PUBLIC YARP::YARP_conf
                                      YARP::YARP_OS
                                      YARP::YARP_sig
                               PRIVATE YARP::YARP_rosmsg)
list(APPEND YARP_dev_PUBLIC_DEPS YARP_conf
                                 YARP_OS
                                 YARP_sig)
list(APPEND YARP_dev_PRIVATE_DEPS YARP_rosmsg)

if(TARGET YARP::YARP_math)
  target_link_libraries(YARP_dev PRIVATE YARP::YARP_math)
  list(APPEND YARP_dev_PRIVATE_DEPS YARP_math)
endif()

target_compile_features(YARP_dev PUBLIC cxx_nullptr
                                        cxx_override
                                        cxx_final)

set_property(TARGET YARP_dev PROPERTY PUBLIC_HEADER ${YARP_dev_HDRS}
                                                    ${YARP_dev_idl_HDRS})
set_property(TARGET YARP_dev PROPERTY PRIVATE_HEADER ${YARP_dev_IMPL_HDRS}
                                                     ${YARP_dev_idl_IMPL_HDRS})
set_property(TARGET YARP_dev PROPERTY VERSION ${YARP_VERSION_SHORT})
set_property(TARGET YARP_dev PROPERTY SOVERSION ${YARP_SOVERSION})

install(TARGETS YARP_dev
        EXPORT YARP_dev
        COMPONENT YARP_dev
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dev"
        PRIVATE_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/yarp/dev/impl")

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_dev
                                 DEPENDENCIES ${YARP_dev_PUBLIC_DEPS}
                                 PRIVATE_DEPENDENCIES ${YARP_dev_PRIVATE_DEPS})

set_property(TARGET YARP_dev PROPERTY FOLDER "Libraries")
