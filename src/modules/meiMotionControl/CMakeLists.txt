# Copyright: (C) 2009 RobotCub Consortium
# Author:  Giorgio Metta, Lorenzo Natale
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(meiMotionControl
                      TYPE yarp::dev::MEIMotionControl
                      INCLUDE MEIMotionControl.h
                      WRAPPER controlboard)
  yarp_install(FILES meimotioncontrol.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

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
endif(NOT SKIP_meiMotionControl)
