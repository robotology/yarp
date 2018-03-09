# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(fakeIMU
                    CATEGORY device
                    TYPE yarp::dev::fakeIMU
                    INCLUDE fakeIMU.h
                    EXTRA_CONFIG WRAPPER=inertial
                    DEPENDS "CREATE_DEVICE_LIBRARY_MODULES;YARP_HAS_MATH_LIB")

if(NOT SKIP_fakeIMU)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(yarp_fakeIMU fakeIMU.cpp
                               fakeIMU.h)
  target_link_libraries(yarp_fakeIMU YARP::YARP_OS
                                     YARP::YARP_sig
                                     YARP::YARP_dev
                                     YARP::YARP_math)
  yarp_install(TARGETS yarp_fakeIMU
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES fakeIMU.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET yarp_fakeIMU PROPERTY FOLDER "Plugins/Device/Fake")
endif()

