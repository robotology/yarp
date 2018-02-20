# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(ovrheadset
                    CATEGORY device
                    TYPE yarp::dev::OVRHeadset
                    INCLUDE OVRHeadset.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_LIBOVR;YARP_HAS_GLFW3;YARP_HAS_GLEW")

if(NOT SKIP_yarpmod_ovrheadset)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # LibOVR Requires C++11
  add_definitions(${CXX11_FLAGS})

  set(yarp_ovrheadset_SRCS OVRHeadset.cpp
                           TextureBuffer.cpp
                           TextureStatic.cpp
                           TextureBattery.cpp
                           InputCallback.cpp
                           GLDebug.cpp)
  set(yarp_ovrheadset_IMGS img-yarp-robot-64.h
                           img-crosshairs.h
                           img-battery-100.h
                           img-battery-080.h
                           img-battery-060.h
                           img-battery-040.h
                           img-battery-caution.h
                           img-battery-low.h
                           img-battery-charging.h
                           img-battery-charging-080.h
                           img-battery-charging-060.h
                           img-battery-charging-040.h
                           img-battery-charging-caution.h
                           img-battery-charging-low.h
                           img-battery-missing.h)
  set(yarp_ovrheadset_HDRS ImageType.h
                           OVRHeadset.h
                           TextureBuffer.h
                           TextureStatic.h
                           TextureBattery.h
                           InputCallback.h
                           GLDebug.h
                           ${yarp_ovrheadset_IMGS})

  source_group("Texture Images" FILES ${yarp_ovrheadset_IMGS})

  yarp_add_plugin(yarp_ovrheadset ${yarp_ovrheadset_SRCS}
                                  ${yarp_ovrheadset_HDRS})

  target_link_libraries(yarp_ovrheadset YARP::YARP_OS
                                        YARP::YARP_sig
                                        YARP::YARP_dev
                                        YARP::YARP_math
                                        LibOVR::OVR
                                        GLEW::GLEW
                                        GLFW3::GLFW3)

  yarp_install(TARGETS yarp_ovrheadset
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES ovrheadset.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_ovrheadset PROPERTY FOLDER "Plugins/Device")
endif()
