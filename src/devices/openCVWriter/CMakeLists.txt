# SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(openCVWriter
  CATEGORY device
  TYPE OpenCVWriter
  INCLUDE OpenCVWriter.h
  GENERATE_PARSER
  EXTRA_CONFIG
    WRAPPER=frameWriter_nws_yarp
  DEPENDS "YARP_HAS_OpenCV"
)

if(NOT SKIP_openCVWriter)
  yarp_add_plugin(yarp_openCVWriter)

  target_sources(yarp_openCVWriter
    PRIVATE
      OpenCVWriter.cpp
      OpenCVWriter.h
      OpenCVWriter_ParamsParser.cpp
      OpenCVWriter_ParamsParser.h
  )

  target_link_libraries(yarp_openCVWriter
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

  target_include_directories(yarp_openCVWriter SYSTEM PRIVATE ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(yarp_openCVWriter PRIVATE ${OpenCV_LIBS})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS OpenCV) (not using targets)

  yarp_install(
    TARGETS yarp_openCVWriter
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_openCVWriter PROPERTY FOLDER "Plugins/Device")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
