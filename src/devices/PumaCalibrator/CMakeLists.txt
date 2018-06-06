# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(pumacalibrator
                    CATEGORY device
                    TYPE yarp::dev::PumaCalibrator
                    INCLUDE PumaCalibrator.h
                    DEPENDS "YARP_COMPILE_UNMAINTAINED"
                    ADVANCED)

if(NOT SKIP_pumacalibrator)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  yarp_add_plugin(pumacalibrator PumaCalibrator.cpp
                                 PumaCalibrator.h)
  yarp_install(TARGETS pumacalibrator
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES pumacalibrator.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set_property(TARGET pumacalibrator PROPERTY FOLDER "Unsupported")
endif(NOT SKIP_pumacalibrator)
