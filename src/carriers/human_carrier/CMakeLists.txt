# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_PLUGIN_LIBRARY)
  yarp_prepare_carrier(human_carrier TYPE HumanCarrier INCLUDE HumanCarrier.h)
  yarp_install(FILES human.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if(NOT SKIP_human_carrier)
  find_package(YARP REQUIRED)
  include_directories(${YARP_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  yarp_add_plugin(yarp_human HumanCarrier.h HumanStream.h HumanCarrier.cpp HumanStream.cpp)
  target_link_libraries(yarp_human YARP_OS YARP_sig)
  target_link_libraries(yarp_human ${ACE_LIBRARIES})
  yarp_install(TARGETS yarp_human
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()
