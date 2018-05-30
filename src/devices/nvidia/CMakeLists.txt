# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(nvidia
                    CATEGORY device
                    TYPE NVIDIAGPU
                    INCLUDE "NVIDIADeviceDriver.h"
                    DEPENDS "YARP_HAS_NVIDIACG")

if(NOT SKIP_nvidia)
  add_definitions(-DGL_GLEXT_PROTOTYPES
                  -DGLX_GLXEXT_PROTOTYPES)

  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  include_directories(SYSTEM ${NVIDIACg_INCLUDE_DIRS})

  yarp_add_plugin(yarp_nvidia NVIDIADeviceDriver.cpp
                              NVIDIADeviceDriver.h
                              FBO_Filter.h)

  target_link_libraries(yarp_nvidia YARP::YARP_OS
                                    YARP::YARP_sig
                                    YARP::YARP_dev
                                    ${NVIDIACg_LIBRARIES})
  yarp_install(TARGETS yarp_nvidia
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES nvidia.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_nvidia PROPERTY FOLDER "Plugins/Device")
endif()
