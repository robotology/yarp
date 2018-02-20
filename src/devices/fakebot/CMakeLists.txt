# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(fakebot
                    CATEGORY device
                    TYPE yarp::dev::FakeBot
                    INCLUDE FakeBot.h
                    DEPENDS CREATE_DEVICE_LIBRARY_MODULES)

if(NOT SKIP_fakebot)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_fakebot FakeBot.cpp
                               FakeBot.h)
  target_link_libraries(yarp_fakebot YARP::YARP_OS
                                     YARP::YARP_sig
                                     YARP::YARP_dev)
  yarp_install(TARGETS yarp_fakebot
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES fakebot.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_fakebot PROPERTY FOLDER "Plugins/Device/Fake")
endif()

