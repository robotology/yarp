# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(multipleanalogsensorsremapper
  CATEGORY device
  TYPE MultipleAnalogSensorsRemapper
  INCLUDE MultipleAnalogSensorsRemapper.h
  DEFAULT ON
)

if(ENABLE_multipleanalogsensorsremapper)
  yarp_add_plugin(yarp_multipleanalogsensorsremapper)

  target_sources(yarp_multipleanalogsensorsremapper
    PRIVATE
      MultipleAnalogSensorsRemapper.cpp
      MultipleAnalogSensorsRemapper.h
  )

  target_sources(yarp_multipleanalogsensorsremapper PRIVATE $<TARGET_OBJECTS:multipleAnalogSensorsSerializations>)
  target_include_directories(yarp_multipleanalogsensorsclient PRIVATE $<TARGET_PROPERTY:multipleAnalogSensorsSerializations,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_multipleanalogsensorsremapper
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

  yarp_install(
    TARGETS yarp_multipleanalogsensorsremapper
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_multipleanalogsensorsremapper PROPERTY FOLDER "Plugins/Device")
endif()
