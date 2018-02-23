# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(FrameTransformServer
                    CATEGORY device
                    TYPE yarp::dev::FrameTransformServer
                    INCLUDE FrameTransformServer.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_MATH_LIB"
                    DEFAULT ON)

if(NOT SKIP_FrameTransformServer)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  yarp_add_plugin(yarp_FrameTransformServer FrameTransformServer.cpp
                                            FrameTransformServer.h
                                            ${yarp_FrameTransformServer_ROS_FILES})
  target_link_libraries(yarp_FrameTransformServer YARP::YARP_OS
                                                  YARP::YARP_sig
                                                  YARP::YARP_dev
                                                  YARP::YARP_math
                                                  YARP::YARP_rosmsg)
  yarp_install(TARGETS yarp_FrameTransformServer
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES FrameTransformServer.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_FrameTransformServer PROPERTY FOLDER "Plugins/Device")
endif()
