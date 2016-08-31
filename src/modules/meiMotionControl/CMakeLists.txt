# Copyright: (C) 2009 RobotCub Consortium
# Author:  Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(meiMotionControl
                    CATEGORY device
                    TYPE yarp::dev::MEIMotionControl
                    INCLUDE MEIMotionControl.h
                    EXTRA_CONFIG WRAPPER=controlboard
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_COMPILE_UNMAINTAINED"
                    ADVANCED ON)

if(NOT SKIP_meiMotionControl)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})
  yarp_add_plugin(yarp_meiMotionControl MEIMotionControl.cpp
                                        MEIMotionControl.h)
  target_link_libraries(yarp_meiMotionControl YARP_OS
                                              YARP_dev)
  yarp_install(TARGETS yarp_meiMotionControl
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES meimotioncontrol.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif(NOT SKIP_meiMotionControl)
