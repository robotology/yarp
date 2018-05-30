# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(human
                    CATEGORY carrier
                    TYPE HumanCarrier
                    INCLUDE HumanCarrier.h
                    EXTRA_CONFIG CODE="HUMANITY")

if(NOT SKIP_human)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_human HumanCarrier.h
                             HumanStream.h
                             HumanCarrier.cpp
                             HumanStream.cpp)
  target_link_libraries(yarp_human YARP::YARP_OS)
  yarp_install(TARGETS yarp_human
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES human.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_human PROPERTY FOLDER "Plugins/Carrier")
endif()
