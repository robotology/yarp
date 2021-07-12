# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

set(CMAKE_SKIP_INSTALL_RPATH ON CACHE BOOL "")
set(YARP_COMPILE_EXAMPLES OFF CACHE BOOL "")
set(YARP_COMPILE_TESTS ON CACHE BOOL "")
set(YARP_DISABLE_FAILING_TESTS ON CACHE BOOL "")
set(YARP_ENABLE_EXAMPLES_AS_TESTS ON CACHE BOOL "")
set(YARP_COMPILE_GUIS ON CACHE BOOL "")
set(YARP_COMPILE_libYARP_math ON CACHE BOOL "")

set(ENABLE_yarpcar_human ON CACHE BOOL "")
set(ENABLE_yarppm_depthimage_to_mono ON CACHE BOOL "")
set(ENABLE_yarppm_depthimage_to_rgb ON CACHE BOOL "")
set(ENABLE_yarppm_segmentationimage_to_rgb ON CACHE BOOL "")
# set(ENABLE_yarpcar_mjpeg ON CACHE BOOL "")

set(ENABLE_yarpmod_fakebot ON CACHE BOOL "")
set(ENABLE_yarpmod_fakeMotionControl ON CACHE BOOL "")
set(ENABLE_yarpmod_fakeAnalogSensor ON CACHE BOOL "")
set(ENABLE_yarpmod_fakeIMU ON CACHE BOOL "")
set(ENABLE_yarpmod_SerialServoBoard ON CACHE BOOL "")
set(ENABLE_yarpmod_serialport ON CACHE BOOL "")
set(ENABLE_yarpmod_imuBosch_BNO055 ON CACHE BOOL "")
set(ENABLE_yarpmod_fakeLaser ON CACHE BOOL "")
set(ENABLE_yarpmod_rpLidar ON CACHE BOOL "")
set(ENABLE_yarpmod_laserHokuyo ON CACHE BOOL "")
set(ENABLE_yarpcar_websocket ON CACHE BOOL "")

