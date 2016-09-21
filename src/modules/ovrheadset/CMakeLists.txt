# Copyright (C) 2015  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(ovrheadset
                    CATEGORY device
                    TYPE yarp::dev::OVRHeadset
                    INCLUDE OVRHeadset.h
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_LIBOVR;YARP_HAS_GLFW3;YARP_HAS_GLEW")

if(NOT SKIP_yarpmod_ovrheadset)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  # LibOVR Requires C++11
  add_definitions(${CXX11_FLAGS})

  set(yarp_ovrheadset_SRCS OVRHeadset.cpp
                           TextureBuffer.cpp
                           InputCallback.cpp)
  set(yarp_ovrheadset_HDRS ImageType.h
                           OVRHeadset.h
                           TextureBuffer.h
                           InputCallback.h)

  yarp_add_plugin(yarp_ovrheadset ${yarp_ovrheadset_SRCS}
                                  ${yarp_ovrheadset_HDRS})

  target_link_libraries(yarp_ovrheadset YARP_OS
                                        YARP_sig
                                        YARP_dev
                                        LibOVR::OVR
                                        GLEW::GLEW
                                        GLFW3::GLFW3)

  yarp_install(TARGETS yarp_ovrheadset
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES ovrheadset.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

endif()
