# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

if(NOT YARP_COMPILE_TESTS)
  return()
endif()

yarp_prepare_plugin(test_segfault
  CATEGORY device
  TYPE SegFault
  INCLUDE SegFault.h
  ADVANCED
)

if(NOT SKIP_test_segfault)
  yarp_add_plugin(yarp_test_segfault)

  target_sources(yarp_test_segfault
    PRIVATE
      SegFault.cpp
      SegFault.h
  )

  target_link_libraries(yarp_test_segfault
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
    TARGETS yarp_test_segfault
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_test_segfault PROPERTY FOLDER "Plugins/Device/Fake")
endif()
