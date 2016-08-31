# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(UNIX OR WIN32)
  if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_plugin(microphone
                        CATEGORY device
                        TYPE yarp::dev::MicrophoneDeviceDriver
                        INCLUDE MicrophoneDeviceDriver.h
                        WRAPPER grabber)
    yarp_install(FILES microphone.ini
                 COMPONENT runtime
                 DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  endif()

  if(NOT SKIP_microphone)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
    get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
    get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
    get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
    include_directories(${YARP_OS_INCLUDE_DIRS}
                        ${YARP_sig_INCLUDE_DIRS}
                        ${YARP_dev_INCLUDE_DIRS})

    if(WIN32)
      set(_SYS Windows)
    else()
      set(_SYS Linux)
    endif()

    yarp_add_plugin(yarp_microphone ${_SYS}MicrophoneDeviceDriver.cpp
                                    MicrophoneDeviceDriver.h)
    target_link_libraries(yarp_microphone YARP_OS
                                          YARP_sig
                                          YARP_dev)
    yarp_install(TARGETS yarp_microphone
                 EXPORT YARP
                 COMPONENT runtime
                 LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                 ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  endif()
endif()
