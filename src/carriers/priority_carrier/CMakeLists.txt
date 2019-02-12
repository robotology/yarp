# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(priority
                    CATEGORY carrier
                    TYPE yarp::os::PriorityCarrier
                    INCLUDE PriorityCarrier.h
                    EXTRA_CONFIG SUBTYPE=recv
                                 CODE="not applicable"
                    DEPENDS "YARP_HAS_MATH_LIB"
                    DEFAULT ON)

if(NOT SKIP_priority)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_priority PriorityCarrier.h
                                PriorityCarrier.cpp)
  target_link_libraries(yarp_priority PRIVATE YARP::YARP_OS
                                              YARP::YARP_sig)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_OS
                                                      YARP_sig)
  if(CREATE_LIB_MATH)
    target_compile_definitions(yarp_priority PRIVATE WITH_YARPMATH)
    target_link_libraries(yarp_priority PRIVATE YARP::YARP_math)
    list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_math)
  endif()

  yarp_install(TARGETS yarp_priority
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_priority PROPERTY FOLDER "Plugins/Carrier")
endif()
