# Copyright (C) 2012 IITRBCS
# Authors: Ali Paikan and Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(priority_carrier TYPE yarp::os::PriorityCarrier INCLUDE PriorityCarrier.h)
  yarp_install(FILES priority.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif (COMPILE_PLUGIN_LIBRARY)

if (NOT SKIP_priority_carrier)

  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})

  yarp_add_plugin(yarp_priority PriorityCarrier.h PriorityCarrier.cpp)

  if(CREATE_LIB_MATH)
    ADD_DEFINITIONS( -DWITH_YARPMATH )
    target_link_libraries(yarp_priority YARP_OS YARP_sig YARP_math)
  else(CREATE_LIB_MATH)
     target_link_libraries(yarp_priority YARP_OS YARP_sig)
  endif(CREATE_LIB_MATH)

  target_link_libraries(yarp_priority ${ACE_LIBRARIES})
  yarp_install(TARGETS yarp_priority
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

endif ()
