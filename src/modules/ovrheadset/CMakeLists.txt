if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(ovrheadset TYPE yarp::dev::OVRHeadset INCLUDE OVRHeadset.h)
  yarp_install(FILES ovrheadset.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()


if(NOT SKIP_yarpmod_ovrheadset)

  include_directories(SYSTEM ${GLEW_INCLUDE_DIRS})
  include_directories(SYSTEM ${YARP_INCLUDE_DIRS})

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

  target_link_libraries(yarp_ovrheadset LibOVR::OVR
                                        YARP_OS
                                        YARP_init
                                        YARP_sig
                                        YARP_dev
                                        ${GLEW_LIBRARIES}
                                        GLFW::glfw3)

  yarp_install(TARGETS yarp_ovrheadset
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

endif()
