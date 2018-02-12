# Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT)
# Authors: Ali Paikan and Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

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
