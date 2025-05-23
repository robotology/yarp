# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(openCVGrabber
  CATEGORY device
  TYPE OpenCVGrabber
  INCLUDE OpenCVGrabber.h
  GENERATE_PARSER
  EXTRA_CONFIG
    WRAPPER=frameGrabber_nws_yarp
  DEPENDS "YARP_HAS_OpenCV"
)

if(NOT SKIP_openCVGrabber)
  yarp_add_plugin(yarp_openCVGrabber)

  target_sources(yarp_openCVGrabber
    PRIVATE
      OpenCVGrabber.cpp
      OpenCVGrabber.h
      OpenCVGrabber_ParamsParser.cpp
      OpenCVGrabber_ParamsParser.h
  )

  target_link_libraries(yarp_openCVGrabber
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

  target_include_directories(yarp_openCVGrabber SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(yarp_openCVGrabber PRIVATE ${OpenCV_LIBS})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS OpenCV) (not using targets)

  yarp_install(
    TARGETS yarp_openCVGrabber
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )
  yarp_install(
    FILES OpenCVGrabber.xml
    DESTINATION ${YARP_MODULES_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_openCVGrabber PROPERTY FOLDER "Plugins/Device")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
