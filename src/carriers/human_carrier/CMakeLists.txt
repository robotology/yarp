# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(human
  CATEGORY carrier
  TYPE HumanCarrier
  INCLUDE HumanCarrier.h
  EXTRA_CONFIG
    CODE="HUMANITY"
)

if(NOT SKIP_human)
  yarp_add_plugin(yarp_human)

  target_sources(yarp_human
    PRIVATE
      HumanCarrier.h
      HumanStream.h
      HumanCarrier.cpp
      HumanStream.cpp
  )

  target_link_libraries(yarp_human PRIVATE YARP::YARP_os)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os)

  yarp_install(
    TARGETS yarp_human
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_human PROPERTY FOLDER "Plugins/Carrier")
endif()
