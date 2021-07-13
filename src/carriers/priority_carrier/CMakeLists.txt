# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(priority
  CATEGORY carrier
  TYPE PriorityCarrier
  INCLUDE PriorityCarrier.h
  EXTRA_CONFIG
    SUBTYPE=recv
    CODE="not applicable"
  DEPENDS "TARGET YARP::YARP_math"
  DEFAULT ON
)

if(NOT SKIP_priority)
  yarp_add_plugin(yarp_priority)

  target_sources(yarp_priority
    PRIVATE
      PriorityCarrier.h
      PriorityCarrier.cpp
  )

  target_link_libraries(yarp_priority
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_math
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_math
  )

  yarp_install(
    TARGETS yarp_priority
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_priority PROPERTY FOLDER "Plugins/Carrier")
endif()
