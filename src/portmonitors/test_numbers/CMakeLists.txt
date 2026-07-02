# SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


yarp_prepare_plugin(test_numbers1
  TYPE Test_numbers1
  INCLUDE Test_numbers1.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
  DEFAULT ON
)

if(SKIP_test_numbers1)
  return()
endif()

yarp_add_plugin(yarp_pm_test_numbers1)
target_sources(yarp_pm_test_numbers1
  PRIVATE
    Test_numbers1.cpp
    Test_numbers1.h
)

target_link_libraries(yarp_pm_test_numbers1
  PRIVATE
    YARP::YARP_os
)

yarp_install(
  TARGETS yarp_pm_test_numbers1
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_test_numbers1 PROPERTY FOLDER "Plugins/Port Monitor")

############################################################################

yarp_prepare_plugin(test_numbers2
  TYPE Test_numbers2
  INCLUDE Test_numbers2.h
  CATEGORY portmonitor
  DEPENDS "ENABLE_yarpcar_portmonitor"
  DEFAULT ON
)

if(SKIP_test_numbers2)
  return()
endif()

yarp_add_plugin(yarp_pm_test_numbers2)

target_sources(yarp_pm_test_numbers2
  PRIVATE
    Test_numbers2.cpp
    Test_numbers2.h
)

target_link_libraries(yarp_pm_test_numbers2
  PRIVATE
    YARP::YARP_os
)

yarp_install(
  TARGETS yarp_pm_test_numbers2
  EXPORT YARP_${YARP_PLUGIN_MASTER}
  COMPONENT ${YARP_PLUGIN_MASTER}
  LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
  ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
  YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
)
set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

set_property(TARGET yarp_pm_test_numbers2 PROPERTY FOLDER "Plugins/Port Monitor")

if(YARP_COMPILE_TESTS)
  add_subdirectory(tests)
endif()
