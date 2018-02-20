# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
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
                    DEPENDS "CREATE_OPTIONAL_CARRIERS"
                    DEFAULT ON)

if(NOT SKIP_priority)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  if(CREATE_LIB_MATH)
    add_definitions(-DWITH_YARPMATH)
  endif()

  yarp_add_plugin(yarp_priority PriorityCarrier.h
                                PriorityCarrier.cpp)
  target_link_libraries(yarp_priority YARP::YARP_OS
                                      YARP::YARP_sig)
  if(CREATE_LIB_MATH)
     target_link_libraries(yarp_priority YARP::YARP_math)
  endif()

  yarp_install(TARGETS yarp_priority
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES priority.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_priority PROPERTY FOLDER "Plugins/Carrier")
endif()
