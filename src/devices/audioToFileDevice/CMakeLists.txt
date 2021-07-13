# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(audioToFileDevice
  CATEGORY device
  TYPE audioToFileDevice
  INCLUDE audioToFileDevice.h
  EXTRA_CONFIG
    WRAPPER=AudioPlayerWrapper
  DEFAULT ON
)

if(NOT SKIP_audioToFileDevice)
  yarp_add_plugin(yarp_audioToFileDevice)

  target_sources(yarp_audioToFileDevice
    PRIVATE
      audioToFileDevice.cpp
      audioToFileDevice.h
  )

  target_link_libraries(yarp_audioToFileDevice
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
    TARGETS yarp_audioToFileDevice
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_audioToFileDevice PROPERTY FOLDER "Plugins/Device")
endif()
