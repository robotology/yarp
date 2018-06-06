# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(meiMotionControl
                    CATEGORY device
                    TYPE yarp::dev::MEIMotionControl
                    INCLUDE MEIMotionControl.h
                    EXTRA_CONFIG WRAPPER=controlboard
                    DEPENDS "YARP_COMPILE_UNMAINTAINED"
                    ADVANCED)

if(NOT SKIP_meiMotionControl)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_meiMotionControl MEIMotionControl.cpp
                                        MEIMotionControl.h)
  target_link_libraries(yarp_meiMotionControl YARP::YARP_OS
                                              YARP::YARP_dev)
  yarp_install(TARGETS yarp_meiMotionControl
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES meimotioncontrol.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_meiMotionControl PROPERTY FOLDER "Unsupported")
endif(NOT SKIP_meiMotionControl)
