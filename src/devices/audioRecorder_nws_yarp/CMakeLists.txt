# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(audioRecorder_nws_yarp
  CATEGORY device
  TYPE AudioRecorder_nws_yarp
  INCLUDE AudioRecorder_nws_yarp.h
  DEFAULT ON
)

if(NOT SKIP_AudioRecorderWrapper)
  yarp_add_plugin(yarp_audioRecorder_nws_yarp)

  target_sources(yarp_audioRecorder_nws_yarp
    PRIVATE
      AudioRecorder_nws_yarp.cpp
      AudioRecorder_nws_yarp.h
      AudioRecorderServerImpl.cpp
      AudioRecorderServerImpl.h
  )

  target_sources(yarp_audioRecorder_nws_yarp PRIVATE $<TARGET_OBJECTS:IAudioGrabberMsgs>)
  target_include_directories(yarp_audioRecorder_nws_yarp PRIVATE $<TARGET_PROPERTY:IAudioGrabberMsgs,INTERFACE_INCLUDE_DIRECTORIES>)

  target_link_libraries(yarp_audioRecorder_nws_yarp
    PRIVATE
      YARP::YARP_os
      YARP::YARP_sig
      YARP::YARP_dev
  )
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS
    YARP_os
    YARP_sig
    YARP_dev
  )

  yarp_install(
    TARGETS yarp_audioRecorder_nws_yarp
    EXPORT YARP_${YARP_PLUGIN_MASTER}
    COMPONENT ${YARP_PLUGIN_MASTER}
    LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
    ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
    YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
  )

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_audioRecorder_nws_yarp PROPERTY FOLDER "Plugins/Device/NWS")

  if(YARP_COMPILE_TESTS)
    add_subdirectory(tests)
  endif()

endif()
