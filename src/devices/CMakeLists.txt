# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(YarpPlugin)
include(YarpPrintFeature)

yarp_begin_plugin_library(yarpmod
  OPTION YARP_COMPILE_DEVICE_PLUGINS
  DEFAULT ON
)
  add_subdirectory(framegrabber_protocol)
  add_subdirectory(audioPlayerWrapper)
  add_subdirectory(audioRecorderWrapper)
  add_subdirectory(audioFromFileDevice)
  add_subdirectory(audioToFileDevice)
  add_subdirectory(depthCamera)
  add_subdirectory(fakeDepthCamera)
  add_subdirectory(fakebot)
  add_subdirectory(fakeMotionControl)
  add_subdirectory(fakeAnalogSensor)
  add_subdirectory(fakeBattery)
  add_subdirectory(fakeIMU)
  add_subdirectory(frameTransformClient)
  add_subdirectory(frameTransformGet)
  add_subdirectory(frameTransformServer)
  add_subdirectory(frameTransformSet)
  add_subdirectory(frameTransformStorageMsgs)
  add_subdirectory(frameTransformStorage)
  add_subdirectory(frameTransformUtils)
  add_subdirectory(transformClient)
  add_subdirectory(transformServer)
  add_subdirectory(SerialServoBoard)
  add_subdirectory(ffmpeg)
  add_subdirectory(opencv)
  add_subdirectory(serialport)
  add_subdirectory(portaudioPlayer)
  add_subdirectory(portaudioRecorder)
  add_subdirectory(imuBosch_BNO055)
  add_subdirectory(DynamixelAX12Ftdi)
  add_subdirectory(fakeLaser)
  add_subdirectory(fakeMicrophone)
  add_subdirectory(fakeSpeaker)
  add_subdirectory(laserFromDepth)
  add_subdirectory(laserFromPointCloud)
  add_subdirectory(laserFromExternalPort)
  add_subdirectory(laserFromRosTopic)
  add_subdirectory(rpLidar)
  add_subdirectory(rpLidar2)
  add_subdirectory(rpLidar3)
  add_subdirectory(laserHokuyo)
  add_subdirectory(fakeFrameGrabber)
  add_subdirectory(SDLJoypad)
  add_subdirectory(batteryClient)
  add_subdirectory(batteryWrapper)
  add_subdirectory(upowerBattery)
  add_subdirectory(Rangefinder2DWrapper)
  add_subdirectory(multipleAnalogSensorsMsgs)
  add_subdirectory(multipleanalogsensorsserver)
  add_subdirectory(multipleanalogsensorsclient)
  add_subdirectory(multipleanalogsensorsremapper)
  add_subdirectory(multipleAnalogSensorsRosPublishers)
  add_subdirectory(localization2DClient)
  add_subdirectory(localization2DServer)
  add_subdirectory(map2DClient)
  add_subdirectory(map2DServer)
  add_subdirectory(map2DStorage)
  add_subdirectory(navigation2DClient)
  add_subdirectory(navigation2DServer)
  add_subdirectory(Rangefinder2DClient)
  add_subdirectory(usbCamera)
  add_subdirectory(fakeLocalizerDevice)
  add_subdirectory(fakeNavigationDevice)
  add_subdirectory(DeviceGroup)
  add_subdirectory(DevicePipe)
  add_subdirectory(ServerSerial)
  add_subdirectory(RemoteFrameGrabber)
  add_subdirectory(RemoteControlBoard)
  add_subdirectory(AnalogSensorClient)
  add_subdirectory(AnalogWrapper)
  add_subdirectory(VirtualAnalogWrapper)
  add_subdirectory(RGBDRosConversionUtils)
  add_subdirectory(RGBDSensorClient)
  add_subdirectory(ServerInertial)
  add_subdirectory(RGBDSensorWrapper)
  add_subdirectory(RGBDToPointCloudSensorWrapper)
  add_subdirectory(RGBDSensorFromRosTopic)
  add_subdirectory(ControlBoardWrapper)
  add_subdirectory(ControlBoardRemapper)
  add_subdirectory(RobotDescriptionClient)
  add_subdirectory(RobotDescriptionServer)
  add_subdirectory(ServerFrameGrabberDual)
  add_subdirectory(JoypadControlNetUtils)
  add_subdirectory(JoypadControlClient)
  add_subdirectory(JoypadControlServer)
  add_subdirectory(frameGrabberCropper)

  add_subdirectory(portaudio) # DEPRECATED Since YARP 3.2
  add_subdirectory(ServerSoundGrabber) # DEPRECATED Since YARP 3.2
  add_subdirectory(ServerFrameGrabber) # DEPRECATED Since YARP 3.3
  add_subdirectory(fakeMotor) # DEPRECATED Since YARP 3.5

  # Test devices
  add_subdirectory(test_segfault)
  add_subdirectory(test_nop)

  # We can also suck in other device libraries built the same way.
  # We seek an ExternalModules.cmake file either in the conf directory
  # or in our build directory
  set(EXTFILES
    "${YARP_MODULE_DIR}/ExternalModules.cmake"
    "${CMAKE_BINARY_DIR}/ExternalModules.cmake"
  )
  foreach(EXTFILE ${EXTFILES})
    if(EXISTS ${EXTFILE})
      include(${EXTFILE})
      foreach(EXTDIR ${EXTERNAL_MODULES})
        if(NOT ${EXTDIR}_DIR)
          set(${EXTDIR}_DIR ${${EXTDIR}_PATH})
        endif()
        if(NOT ${EXTDIR}_DIR)
          message(FATAL_ERROR "Need ${EXTDIR}_DIR in ${EXTFILE}")
        endif()
        # make sure path is usable - could have environment variable parts
        file(TO_CMAKE_PATH ${${EXTDIR}_DIR} safe_path)
        if(NOT EXISTS "${safe_path}/CMakeLists.txt")
          message(FATAL_ERROR "${EXTDIR}_DIR=${${EXTDIR}_DIR} from ${EXTFILE} does not contain a CMakeLists.txt")
        endif()
        add_subdirectory(${safe_path}
                         ${CMAKE_CURRENT_BINARY_DIR}/externals/${EXTDIR})
      endforeach()
    endif()
  endforeach()

yarp_end_plugin_library(yarpmod QUIET)
add_library(YARP::yarpmod ALIAS yarpmod)


install(
  TARGETS yarpmod
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_yarpmod
  DEPENDENCIES ${YARP_yarpmod_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_yarpmod_PRIVATE_DEPS}
)

set_property(TARGET yarpmod PROPERTY FOLDER "Plugins/Device")

yarp_renamed_option(CREATE_DEVICE_LIBRARY_MODULES YARP_COMPILE_DEVICE_PLUGINS) # since YARP 3.0.0
