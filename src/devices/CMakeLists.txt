# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(YarpPlugin)
include(YarpPrintFeature)
include(YarpCatchUtils)

yarp_begin_plugin_library(yarpmod
  OPTION YARP_COMPILE_DEVICE_PLUGINS
  DEFAULT ON
)
  add_subdirectory(framegrabber_protocol)
  add_subdirectory(audioPlayerWrapper)
  add_subdirectory(audioRecorderWrapper)
  add_subdirectory(audioRecorder_nwc_yarp)
  add_subdirectory(audioRecorder_nws_yarp)
  add_subdirectory(audioFromFileDevice)
  add_subdirectory(audioToFileDevice)
  add_subdirectory(openNI2DepthCamera)
  add_subdirectory(fakeDepthCamera)
  add_subdirectory(fakebot)
  add_subdirectory(fakeChatBotDevice)
  add_subdirectory(fakeMotionControl)
  add_subdirectory(fakeAnalogSensor)
  add_subdirectory(fakeBattery)
  add_subdirectory(fakeIMU)
  add_subdirectory(fakeJoypad)
  add_subdirectory(fakeLLMDevice)
  add_subdirectory(fakeOdometry2D)
  add_subdirectory(fakePositionSensor)
  add_subdirectory(fakeSerialPort)
  add_subdirectory(fakeSpeechSynthesizer)
  add_subdirectory(fakeSpeechTranscription)
  add_subdirectory(frameTransformClient)
  add_subdirectory(frameTransformGet)
  add_subdirectory(frameTransformServer)
  add_subdirectory(frameTransformSet)
  add_subdirectory(frameTransformStorageMsgs)
  add_subdirectory(frameTransformStorage)
  add_subdirectory(frameGrabber_nwc_yarp)
  add_subdirectory(frameGrabber_nws_yarp)
  add_subdirectory(SerialServoBoard)
  add_subdirectory(ffmpeg)
  add_subdirectory(opencv)
  add_subdirectory(serialport)
  add_subdirectory(portaudioPlayer)
  add_subdirectory(portaudioRecorder)
  add_subdirectory(imuBosch_BNO055)
  add_subdirectory(IChatBotMsgs)
  add_subdirectory(ILLMMsgs)
  add_subdirectory(IAudioGrabberMsgs)
  add_subdirectory(IMap2DMsgs)
  add_subdirectory(INavigation2DMsgs)
  add_subdirectory(ILocalization2DMsgs)
  add_subdirectory(IOdometry2DMsgs)
  add_subdirectory(ISerialMsgs)
  add_subdirectory(ISpeechSynthesizerMsgs)
  add_subdirectory(ISpeechTranscriptionMsgs)
  add_subdirectory(DynamixelAX12Ftdi)
  add_subdirectory(fakeLaser)
  add_subdirectory(fakeLaserWithMotor)
  add_subdirectory(fakeMicrophone)
  add_subdirectory(fakeSpeaker)
  add_subdirectory(laserFromDepth)
  add_subdirectory(laserFromPointCloud)
  add_subdirectory(laserFromExternalPort)
  add_subdirectory(laserHokuyo)
  add_subdirectory(fakeFrameGrabber)
  add_subdirectory(SDLJoypad)
  add_subdirectory(battery_nwc_yarp)
  add_subdirectory(battery_nws_yarp)
  add_subdirectory(chatBot_nwc_yarp)
  add_subdirectory(chatBot_nws_yarp)
  add_subdirectory(upowerBattery)
  add_subdirectory(Rangefinder2D_nws_yarp)
  add_subdirectory(Rangefinder2D_nwc_yarp)
  add_subdirectory(mobileBaseVelocityControl_nwc_yarp)
  add_subdirectory(mobileBaseVelocityControl_nws_yarp)
  add_subdirectory(mobileBaseVelocityControlMsgs)
  add_subdirectory(multipleAnalogSensorsMsgs)
  add_subdirectory(multipleanalogsensorsserver)
  add_subdirectory(multipleanalogsensorsclient)
  add_subdirectory(multipleanalogsensorsremapper)
  add_subdirectory(LLM_nws_yarp)
  add_subdirectory(LLM_nwc_yarp)
  add_subdirectory(localization2D_nwc_yarp)
  add_subdirectory(localization2D_nws_yarp)
  add_subdirectory(map2D_nwc_yarp)
  add_subdirectory(map2D_nws_yarp)
  add_subdirectory(map2DStorage)
  add_subdirectory(navigation2D_nwc_yarp)
  add_subdirectory(navigation2D_nws_yarp)
  add_subdirectory(odometry2D_nws_yarp)
  add_subdirectory(Rangefinder2DClient)
  add_subdirectory(usbCamera)
  add_subdirectory(fakeLocalizerDevice)
  add_subdirectory(fakeNavigationDevice)
  add_subdirectory(serialPort_nws_yarp)
  add_subdirectory(serialPort_nwc_yarp)
  add_subdirectory(speechSynthesizer_nws_yarp)
  add_subdirectory(speechSynthesizer_nwc_yarp)
  add_subdirectory(speechTranscription_nws_yarp)
  add_subdirectory(speechTranscription_nwc_yarp)
  add_subdirectory(RemoteControlBoard)
  add_subdirectory(AnalogSensorClient)
  add_subdirectory(AnalogWrapper)
  add_subdirectory(VirtualAnalogWrapper)
  add_subdirectory(RGBDSensorClient)
  add_subdirectory(RGBDSensor_nws_yarp)
  add_subdirectory(controlBoard_nws_yarp)
  add_subdirectory(controlBoardRemapper)
  add_subdirectory(RobotDescriptionClient)
  add_subdirectory(RobotDescriptionServer)
  add_subdirectory(JoypadControlNetUtils)
  add_subdirectory(JoypadControlClient)
  add_subdirectory(JoypadControlServer)
  add_subdirectory(frameGrabberCropper)

  add_subdirectory(portaudio) # DEPRECATED Since YARP 3.2

  OPTION(YARP_COMPILE_ALL_FAKE_DEVICES
         "Build all devices marked as fake, for testing purposes. Overrides the option for the single fake device."
         OFF)
  mark_as_advanced(YARP_COMPILE_ALL_FAKE_DEVICES)

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

set_property(TARGET yarpmod PROPERTY FOLDER "Plugins")
