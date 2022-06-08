# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(opencv_grabber
  CATEGORY device
  TYPE OpenCVGrabber
  INCLUDE OpenCVGrabber.h
  EXTRA_CONFIG
    WRAPPER=grabberDual
  DEPENDS "YARP_HAS_OpenCV"
)

if(NOT SKIP_opencv_grabber)
  yarp_add_plugin(yarp_opencv)

  target_sources(yarp_opencv
    PRIVATE
      OpenCVGrabber.cpp
      OpenCVGrabber.h
  )

  target_link_libraries(yarp_opencv
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  target_include_directories(yarp_opencv SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(yarp_opencv PRIVATE ${OpenCV_LIBRARIES})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS OpenCV) (not using targets)

  yarp_install(
    TARGETS yarp_opencv
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )
  yarp_install(
    FILES opencv_grabber.xml
    DESTINATION ${YARP_MODULES_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_opencv PROPERTY FOLDER "Plugins/Device")
endif()

include(YarpRemoveFile)
yarp_remove_file("${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}/opencv.ini") # YARP_DEPRECATED since YARP 3.2
