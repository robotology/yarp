# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(WIN32)
  if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(vfw_grabber
                        TYPE yarp::dev::VfwGrabber
                        INCLUDE VfwGrabber.h
                        WRAPPER grabber)
    yarp_install(FILES vfw.ini
                COMPONENT runtime
                DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
  endif()

  if(NOT SKIP_vfw_grabber)
    find_library(vfw_library vfw32)
    if(vfw_library)

      set(CMAKE_INCLUDE_CURRENT_DIR ON)
      get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
      get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
      get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
      include_directories(${YARP_OS_INCLUDE_DIRS}
                          ${YARP_sig_INCLUDE_DIRS}
                          ${YARP_dev_INCLUDE_DIRS})
      yarp_add_plugin(yarp_vfw VfwGrabber.cpp
                               VfwGrabber.h)
      target_link_libraries(yarp_vfw YARP_OS
                                     YARP_sig
                                     YARP_dev
                                     ${vfw_library})
      yarp_install(TARGETS yarp_vfw
                   EXPORT YARP
                   COMPONENT runtime
                   LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
                   ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})

      if(YARP_COMPILE_TESTS)
        add_executable(vfw_test vfw_test.cpp
                                VfwGrabber.cpp
                                VfwGrabber.h)
        target_link_libraries(vfw_test YARP_OS
                                      YARP_init
                                      YARP_sig
                                      YARP_dev
                                      ${vfw_library})
      endif()

    else()
      message(FATAL_ERROR "cannot find video for windows")
    endif()

  endif()
endif()
