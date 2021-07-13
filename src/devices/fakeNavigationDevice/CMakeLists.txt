# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(fakeNavigation
  CATEGORY device
  TYPE fakeNavigation
  INCLUDE fakeNavigationDev.h
  DEPENDS "TARGET YARP::YARP_math"
)

if(NOT SKIP_fakeNavigation)
  yarp_add_plugin(yarp_fakeNavigation)

  target_sources(yarp_fakeNavigation
    PRIVATE
      fakeNavigationDev.h
      fakeNavigationDev.cpp
    )

  target_link_libraries(yarp_fakeNavigation
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
      YARP::YARP_math
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
    YARP_math
  )

  yarp_install(
    TARGETS yarp_fakeNavigation
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_fakeNavigation PROPERTY FOLDER "Plugins/Device/Fake")
endif()
