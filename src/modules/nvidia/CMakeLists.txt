# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(COMPILE_DEVICE_LIBRARY)
  yarp_prepare_device(nvidia
                      TYPE NVIDIAGPU
                      INCLUDE "NVIDIADeviceDriver.h")
  yarp_install(FILES nvidia.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})
endif()

if(NOT SKIP_nvidia)
  add_definitions(-DGL_GLEXT_PROTOTYPES
                  -DGLX_GLXEXT_PROTOTYPES)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_dev_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_dev_INCLUDE_DIRS})

  include_directories(SYSTEM ${NVIDIACg_INCLUDE_DIRS})

  yarp_add_plugin(yarp_nvidia NVIDIADeviceDriver.cpp
                              NVIDIADeviceDriver.h
                              FBO_Filter.h)

  target_link_libraries(yarp_nvidia YARP_OS
                                    YARP_sig
                                    YARP_dev
                                    ${NVIDIACg_LIBRARIES})
  yarp_install(TARGETS yarp_nvidia
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
endif()
