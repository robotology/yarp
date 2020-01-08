# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(IMURosPublisher
                    CATEGORY device
                    TYPE IMURosPublisher
                    INCLUDE IMURosPublisher.h
                    EXTRA_CONFIG WRAPPER=IMURosPublisher
                    DEFAULT ON)

if(ENABLE_IMURosPublisher)
  yarp_add_plugin(yarp_IMURosPublisher)

  target_sources(yarp_IMURosPublisher PRIVATE IMURosPublisher.cpp IMURosPublisher.h GenericSensorRosPublisher.h )

  target_link_libraries(yarp_IMURosPublisher PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev
                                                     YARP::YARP_math
                                                     YARP_rosmsg)
                                                     
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_IMURosPublisher
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_IMURosPublisher PROPERTY FOLDER "Plugins/Device")
endif()

#---------------------------------------------------------------------------------------------------------

yarp_prepare_plugin(WrenchRosPublisher
                    CATEGORY device
                    TYPE WrenchRosPublisher
                    INCLUDE WrenchRosPublisher.h
                    EXTRA_CONFIG WRAPPER=WrenchRosPublisher
                    DEFAULT ON)

if(ENABLE_WrenchRosPublisher)
  yarp_add_plugin(yarp_WrenchRosPublisher)

  target_sources(yarp_WrenchRosPublisher PRIVATE WrenchRosPublisher.cpp WrenchRosPublisher.h GenericSensorRosPublisher.h )

  target_link_libraries(yarp_WrenchRosPublisher PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev
                                                     YARP_rosmsg)
                                                     
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_WrenchRosPublisher
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_WrenchRosPublisher PROPERTY FOLDER "Plugins/Device")
endif()

#---------------------------------------------------------------------------------------------------------

yarp_prepare_plugin(TemperatureRosPublisher
                    CATEGORY device
                    TYPE TemperatureRosPublisher
                    INCLUDE TemperatureRosPublisher.h
                    EXTRA_CONFIG WRAPPER=TemperatureRosPublisher
                    DEFAULT ON)

if(ENABLE_TemperatureRosPublisher)
  yarp_add_plugin(yarp_TemperatureRosPublisher)

  target_sources(yarp_TemperatureRosPublisher PRIVATE TemperatureRosPublisher.cpp TemperatureRosPublisher.h GenericSensorRosPublisher.h )

  target_link_libraries(yarp_TemperatureRosPublisher PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev
                                                     YARP_rosmsg)
                                                     
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_TemperatureRosPublisher
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_TemperatureRosPublisher PROPERTY FOLDER "Plugins/Device")
endif()

#---------------------------------------------------------------------------------------------------------

yarp_prepare_plugin(PoseRosPublisher
                    CATEGORY device
                    TYPE PoseRosPublisher
                    INCLUDE PoseRosPublisher.h
                    EXTRA_CONFIG WRAPPER=PoseRosPublisher
                    DEFAULT ON)

if(ENABLE_PoseRosPublisher)
  yarp_add_plugin(yarp_PoseRosPublisher)

  target_sources(yarp_PoseRosPublisher PRIVATE PoseRosPublisher.cpp PoseRosPublisher.h GenericSensorRosPublisher.h )

  target_link_libraries(yarp_PoseRosPublisher PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev
                                                     YARP::YARP_math
                                                     YARP_rosmsg)
                                                     
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_math
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_PoseRosPublisher
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_PoseRosPublisher PROPERTY FOLDER "Plugins/Device")
endif()

#---------------------------------------------------------------------------------------------------------

yarp_prepare_plugin(MagfieldRosPublisher
                    CATEGORY device
                    TYPE MagfieldRosPublisher
                    INCLUDE MagfieldRosPublisher.h
                    EXTRA_CONFIG WRAPPER=MagfieldRosPublisher
                    DEFAULT ON)

if(ENABLE_MagfieldRosPublisher)
  yarp_add_plugin(yarp_MagfieldRosPublisher)

  target_sources(yarp_MagfieldRosPublisher PRIVATE MagfieldRosPublisher.cpp MagfieldRosPublisher.h GenericSensorRosPublisher.h )

  target_link_libraries(yarp_MagfieldRosPublisher PRIVATE YARP::YARP_os
                                                     YARP::YARP_sig
                                                     YARP::YARP_dev
                                                     YARP_rosmsg)
                                                     
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_dev
                                                      YARP_rosmsg)

  yarp_install(TARGETS yarp_MagfieldRosPublisher
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_MagfieldRosPublisher PROPERTY FOLDER "Plugins/Device")
endif()

#---------------------------------------------------------------------------------------------------------