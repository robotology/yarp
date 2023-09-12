# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(chatBot_nwc_yarp
  CATEGORY device
  TYPE ChatBot_nwc_yarp
  INCLUDE ChatBot_nwc_yarp.h
  DEFAULT ON
)

if(NOT SKIP_chatBot_nwc_yarp)
  yarp_add_plugin(chatBot_nwc_yarp)

  target_sources(chatBot_nwc_yarp
  PRIVATE
  ChatBot_nwc_yarp.cpp
  ChatBot_nwc_yarp.h
  )

  target_sources(chatBot_nwc_yarp PRIVATE $<TARGET_OBJECTS:IChatBotMsgs>)

  target_include_directories(chatBot_nwc_yarp PRIVATE $<TARGET_PROPERTY:IChatBotMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(chatBot_nwc_yarp
  PRIVATE
      YARP::YARP_os
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
      YARP_os
      YARP_dev
  )

  yarp_install(
  TARGETS chatBot_nwc_yarp
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET chatBot_nwc_yarp PROPERTY FOLDER "Plugins/Device/NWC")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
