# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_SKIP_INSTALL_RPATH ON CACHE BOOL "")
set(YARP_COMPILE_EXAMPLES ON CACHE BOOL "")
set(YARP_COMPILE_TESTS ON CACHE BOOL "")
set(YARP_ENABLE_EXAMPLES_AS_TESTS ON CACHE BOOL "")
set(YARP_COMPILE_GUIS ON CACHE BOOL "")
set(YARP_COMPILE_yarpviz ON CACHE BOOL "")
set(YARP_COMPILE_libYARP_math ON CACHE BOOL "")
set(YARP_COMPILE_ALL_FAKE_DEVICES ON CACHE BOOL "")

set(ENABLE_yarpcar_shmem ON CACHE BOOL "")
set(ENABLE_yarpcar_human ON CACHE BOOL "")
set(ENABLE_yarpcar_mpi ON CACHE BOOL "")
set(ENABLE_yarpcar_mpibcast ON CACHE BOOL "")
set(ENABLE_yarpcar_xmlrpc ON CACHE BOOL "")
set(ENABLE_yarpcar_tcpros ON CACHE BOOL "")
set(ENABLE_yarpcar_rossrv ON CACHE BOOL "")
set(ENABLE_yarpcar_mjpeg ON CACHE BOOL "")
set(ENABLE_yarpcar_bayer ON CACHE BOOL "")
set(ENABLE_yarpcar_priority ON CACHE BOOL "")
set(ENABLE_yarpcar_portmonitor ON CACHE BOOL "")
set(ENABLE_yarpcar_gstreamer ON CACHE BOOL "" ON CACHE BOOL "")
set(ENABLE_yarpcar_unix_stream ON CACHE BOOL "")
set(ENABLE_yarpcar_websocket ON CACHE BOOL "")

set(ENABLE_yarppm_depthimage_to_mono ON CACHE BOOL "")
set(ENABLE_yarppm_depthimage_to_rgb ON CACHE BOOL "")
set(ENABLE_yarppm_segmentationimage_to_rgb ON CACHE BOOL "")
set(ENABLE_yarppm_image_compression_ffmpeg ON CACHE BOOL "")
set(ENABLE_yarppm_sound_compression_mp3 ON CACHE BOOL "")
set(ENABLE_yarppm_sound_marker ON CACHE BOOL "")
set(ENABLE_yarppm_soundfilter_resample ON CACHE BOOL "")

set(ENABLE_yarpmod_AudioPlayerWrapper ON CACHE BOOL "")
set(ENABLE_yarpmod_AudioRecorderWrapper ON CACHE BOOL "")
set(ENABLE_yarpmod_depthCamera ON CACHE BOOL "")
set(ENABLE_yarpmod_frameGrabberCropper ON CACHE BOOL "")
set(ENABLE_yarpmod_SerialServoBoard ON CACHE BOOL "")
set(ENABLE_yarpmod_openCVGrabber ON CACHE BOOL "")
set(ENABLE_yarpmod_openCVWriter ON CACHE BOOL "")
set(ENABLE_yarpmod_serialport ON CACHE BOOL "")
set(ENABLE_yarpmod_portaudioPlayer OFF CACHE BOOL "")
set(ENABLE_yarpmod_portaudioRecorder ON CACHE BOOL "")
set(ENABLE_yarpmod_imuBosch_BNO055 ON CACHE BOOL "")
set(ENABLE_yarpmod_dynamixelAX12Ftdi ON CACHE BOOL "")
set(ENABLE_yarpmod_laserFromDepth ON CACHE BOOL "")
set(ENABLE_yarpmod_laserFromPointCloud ON CACHE BOOL "")
set(ENABLE_yarpmod_laserFromExternalPort ON CACHE BOOL "")
set(ENABLE_yarpmod_laserFromRosTopic ON CACHE BOOL "")
set(ENABLE_yarpmod_rpLidar ON CACHE BOOL "")
set(ENABLE_yarpmod_rpLidar2 ON CACHE BOOL "")
set(ENABLE_yarpmod_rpLidar3 ON CACHE BOOL "")
set(ENABLE_yarpmod_laserHokuyo ON CACHE BOOL "")
set(ENABLE_yarpmod_test_grabber ON CACHE BOOL "")
set(ENABLE_yarpmod_SDLJoypad ON CACHE BOOL "")
set(ENABLE_yarpmod_batteryClient ON CACHE BOOL "")
set(ENABLE_yarpmod_batteryWrapper ON CACHE BOOL "")
set(ENABLE_yarpmod_upowerBattery ON CACHE BOOL "")
set(ENABLE_yarpmod_rangefinder2D_nws_yarp ON CACHE BOOL "")
set(ENABLE_yarpmod_multipleanalogsensorsserver ON CACHE BOOL "")
set(ENABLE_yarpmod_multipleanalogsensorsclient ON CACHE BOOL "")
set(ENABLE_yarpmod_multipleanalogsensorsremapper ON CACHE BOOL "")
set(ENABLE_yarpmod_IMURosPublisher ON CACHE BOOL "")
set(ENABLE_yarpmod_WrenchStampedRosPublisher ON CACHE BOOL "")
set(ENABLE_yarpmod_TemperatureRosPublisher ON CACHE BOOL "")
set(ENABLE_yarpmod_PoseStampedRosPublisher ON CACHE BOOL "")
set(ENABLE_yarpmod_MagneticFieldRosPublisher ON CACHE BOOL "")
set(ENABLE_yarpmod_transformClient ON CACHE BOOL "")
set(ENABLE_yarpmod_transformServer ON CACHE BOOL "")
set(ENABLE_yarpmod_localization2DClient ON CACHE BOOL "")
set(ENABLE_yarpmod_localization2DServer ON CACHE BOOL "")
set(ENABLE_yarpmod_map2DClient ON CACHE BOOL "")
set(ENABLE_yarpmod_map2DServer ON CACHE BOOL "")
set(ENABLE_yarpmod_navigation2DClient ON CACHE BOOL "")
set(ENABLE_yarpmod_navigation2DServer ON CACHE BOOL "")
set(ENABLE_yarpmod_Rangefinder2DClient ON CACHE BOOL "")
set(ENABLE_yarpmod_usbCamera ON CACHE BOOL "")
set(ENABLE_yarpmod_usbCameraRaw ON CACHE BOOL "")
set(ENABLE_yarpmod_group ON CACHE BOOL "")
set(ENABLE_yarpmod_pipe ON CACHE BOOL "")
set(ENABLE_yarpmod_serial ON CACHE BOOL "")
set(ENABLE_yarpmod_test_motor ON CACHE BOOL "")
set(ENABLE_yarpmod_frameGrabber_nwc_yarp ON CACHE BOOL "")
set(ENABLE_yarpmod_frameGrabber_nws_yarp ON CACHE BOOL "")
set(ENABLE_yarpmod_remote_controlboard ON CACHE BOOL "")
set(ENABLE_yarpmod_analogsensorclient ON CACHE BOOL "")
set(ENABLE_yarpmod_analogServer ON CACHE BOOL "")
set(ENABLE_yarpmod_virtualAnalogServer ON CACHE BOOL "")
set(ENABLE_yarpmod_RGBDSensorClient ON CACHE BOOL "")
set(ENABLE_yarpmod_inertial ON CACHE BOOL "")
set(ENABLE_yarpmod_RGBDSensorWrapper ON CACHE BOOL "")
set(ENABLE_yarpmod_controlboard_nws_yarp ON CACHE BOOL "")
set(ENABLE_yarpmod_controlboardremapper ON CACHE BOOL "")
set(ENABLE_yarpmod_remotecontrolboardremapper ON CACHE BOOL "")
set(ENABLE_yarpmod_robotDescription_nwc_yarp ON CACHE BOOL "")
set(ENABLE_yarpmod_robotDescription_nws_yarp ON CACHE BOOL "")
set(ENABLE_yarpmod_robotDescriptionStorage ON CACHE BOOL "")
set(ENABLE_yarpmod_JoypadControlClient ON CACHE BOOL "")
set(ENABLE_yarpmod_JoypadControlServer ON CACHE BOOL "")
set(ENABLE_yarpmod_portaudio OFF CACHE BOOL "")
set(ENABLE_yarpmod_ServerSoundGrabber ON CACHE BOOL "")
set(ENABLE_yarpmod_grabber ON CACHE BOOL "")
set(ENABLE_yarpmod_test_segfault ON CACHE BOOL "")
set(ENABLE_yarpmod_test_nop ON CACHE BOOL "")

include(${CMAKE_CURRENT_LIST_DIR}/bindings-cache.cmake)
