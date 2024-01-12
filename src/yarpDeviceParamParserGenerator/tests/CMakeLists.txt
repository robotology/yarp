# SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include(YarpPlugin)
include(YarpPrintFeature)
include(YarpCatchUtils)
include(YarpDeviceParamsGenerator)

yarp_prepare_plugin(testDeviceWGP
  CATEGORY device
  TYPE TestDeviceWGP
  INCLUDE TestDeviceWGP.h
  DEFAULT ON
)

generateDeviceParamsParser(TestDeviceWGP)

yarp_add_plugin(yarp_testDeviceWGP)

target_sources(yarp_testDeviceWGP
    PRIVATE
      TestDeviceWGP.cpp
      TestDeviceWGP.h
      TestDeviceWGPParams.cpp
      TestDeviceWGPParams.h
)

target_link_libraries(yarp_testDeviceWGP
    PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
)
list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_dev
)

  yarp_install(
    TARGETS yarp_testDeviceWGP
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

source_group("OtherFiles" FILES
      testDeviceWGPParams.ini
      testDeviceWGPParams.md
      testDeviceWGPParams.xml
)

set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_testDeviceWGP PROPERTY FOLDER "Plugins/Device")
