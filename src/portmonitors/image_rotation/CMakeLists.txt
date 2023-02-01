# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(image_rotation
  TYPE ImageRotation
  INCLUDE imageRotation.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
)

if(SKIP_image_rotation)
  return()
endif()

yarp_add_plugin(yarp_pm_image_rotation)

target_sources(yarp_pm_image_rotation
  PRIVATE
    imageRotation.cpp
    imageRotation.h
)
target_link_libraries(yarp_pm_image_rotation
  PRIVATE
    YARP::YARP_os
    YARP::YARP_sig
    YARP::YARP_cv
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
  YARP_os
  YARP_sig
)

 target_include_directories(yarp_pm_image_rotation SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})
 target_link_libraries(yarp_pm_image_rotation PRIVATE ${OpenCV_LIBRARIES})

yarp_install(
  TARGETS yarp_pm_image_rotation
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_image_rotation PROPERTY FOLDER "Plugins/Port Monitor")
