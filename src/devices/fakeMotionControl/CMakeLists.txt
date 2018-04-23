# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(fakeMotionControl
                    CATEGORY device
                    TYPE yarp::dev::FakeMotionControl
                    INCLUDE fakeMotionControl.h
                    EXTRA_CONFIG WRAPPER=controlboardwrapper2
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_MATH_LIB")

if(NOT SKIP_fakeMotionControl)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_fakeMotionControl fakeMotionControl.cpp
                                         fakeMotionControl.h)
  target_link_libraries(yarp_fakeMotionControl YARP::YARP_OS
                                               YARP::YARP_sig
                                               YARP::YARP_dev
                                               YARP::YARP_math)
  yarp_install(TARGETS yarp_fakeMotionControl
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES fakeMotionControl.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_fakeMotionControl PROPERTY FOLDER "Plugins/Device/Fake")
endif()

