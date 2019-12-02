# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

# YARP CMake Modules
set(YARP_CMAKE_FILES YarpPlugin.cmake
                     YarpIDL.cmake
                     YarpInstallationHelpers.cmake
                     YarpPrintFeature.cmake
                     YarpDeprecatedOption.cmake)
install(FILES ${YARP_CMAKE_FILES}
        DESTINATION share/yarp/cmake
        COMPONENT YARP_conf-dev)

# Templates used by modules
set(YARP_TEMPLATE_FILES template/yarp_plugin_carrier.cpp.in
                        template/yarp_plugin_RFModule.cpp.in
                        template/yarp_plugin_device.cpp.in
                        template/yarp_plugin_portmonitor.cpp.in
                        template/yarp_plugin_library.cpp.in
                        template/yarp_plugin_yarpdev_main.cpp.in)

install(FILES ${YARP_TEMPLATE_FILES}
        DESTINATION share/yarp/cmake/template
        COMPONENT YARP_conf-dev)

# Remove renamed files from installation
include(YarpRemoveFile)
foreach(_f YarpDeprecatedWarning.cmake   # since YARP 3.0.0
           deprecated/YarpRenamedOption.cmake
           deprecated/YarpBackupVariable.cmake)
  yarp_remove_file("share/yarp/cmake/${_f}")
endforeach()
