# Copyright (C) 2017 Marco Randazzo
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(rpLidar2
                    CATEGORY device
                    TYPE RpLidar2
                    INCLUDE rpLidar2.h
                    DEPENDS CREATE_DEVICE_LIBRARY_MODULES)

if(NOT SKIP_rpLidar2)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  set(RPLIDAR_SDK_ROOT ${CMAKE_SOURCE_DIR}/extern/rplidar/sdk)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS}
                      ${RPLIDAR_SDK_ROOT}/include
                      ${RPLIDAR_SDK_ROOT}/src)

  if (WIN32) 
  set (ARCH_DIR win32)
  elseif (APPLE)
  set (ARCH_DIR macOS)
  elseif (UNIX)
  set (ARCH_DIR linux)
  endif ()


  yarp_add_plugin(rpLidar2 rpLidar2.h
                           rpLidar2.cpp
                           ${RPLIDAR_SDK_ROOT}/src/rplidar_driver.cpp
                           ${RPLIDAR_SDK_ROOT}/src/arch/${ARCH_DIR}/net_serial.cpp
                           ${RPLIDAR_SDK_ROOT}/src/arch/${ARCH_DIR}/timer.cpp
                           ${RPLIDAR_SDK_ROOT}/src/hal/thread.cpp)

  target_link_libraries(rpLidar2 YARP::YARP_OS
                                YARP::YARP_sig
                                YARP::YARP_dev)
  yarp_install(TARGETS rpLidar2
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES rpLidar2.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()
