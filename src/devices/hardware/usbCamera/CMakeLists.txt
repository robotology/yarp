# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(usbCamera
  CATEGORY device
  TYPE USBCameraDriverRgb
  INCLUDE common/USBcamera.h
  EXTRA_CONFIG
    WRAPPER=grabberDual
  DEPENDS "UNIX;NOT APPLE;YARP_HAS_Libv4l2;YARP_HAS_Libv4lconvert;YARP_HAS_OpenCV"
)

yarp_prepare_plugin(usbCameraRaw
  CATEGORY device
  TYPE USBCameraDriverRaw
  INCLUDE common/USBcamera.h
  EXTRA_CONFIG
    WRAPPER=grabberDual
  DEPENDS "UNIX;NOT APPLE;YARP_HAS_Libv4l2;YARP_HAS_Libv4lconvert;YARP_HAS_OpenCV"
)

if(ENABLE_usbCamera OR ENABLE_usbCameraRaw)
  yarp_add_plugin(yarp_usbCamera)

  target_sources(yarp_usbCamera
    PRIVATE
      common/USBcamera.cpp
      common/USBcamera.h
      common/USBcameraLogComponent.cpp
      common/USBcameraLogComponent.h
      linux/V4L_camera.cpp
      linux/V4L_camera.h
      linux/list.cpp
      linux/list.h
  )

  target_include_directories(yarp_usbCamera PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/common")
  target_include_directories(yarp_usbCamera PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/linux")

  target_link_libraries(yarp_usbCamera PRIVATE YARP::YARP_os
                                               YARP::YARP_sig
                                               YARP::YARP_dev)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev)

  target_include_directories(yarp_usbCamera PRIVATE ${Libv4l2_INCLUDE_DIRS})
  target_link_libraries(yarp_usbCamera PRIVATE ${Libv4l2_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS Libv4l2) (not using targets)

  target_include_directories(yarp_usbCamera PRIVATE ${Libv4lconvert_INCLUDE_DIRS})
  target_link_libraries(yarp_usbCamera PRIVATE ${Libv4lconvert_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS Libv4lconvert) (not using targets)

  target_include_directories(yarp_usbCamera SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(yarp_usbCamera PRIVATE ${OpenCV_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS OpenCV) (not using targets)

  yarp_install(
    TARGETS yarp_usbCamera
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_usbCamera PROPERTY FOLDER "Plugins/Device")

endif()
