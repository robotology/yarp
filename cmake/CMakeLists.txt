# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# YARP CMake Modules
set(YARP_CMAKE_FILES
  YarpPlugin.cmake
  YarpIDL.cmake
  YarpInstallationHelpers.cmake
  YarpPrintFeature.cmake
  YarpDeprecatedOption.cmake
)
install(
  FILES ${YARP_CMAKE_FILES}
  DESTINATION share/yarp/cmake
  COMPONENT YARP_conf-dev
)

# Templates used by modules
set(YARP_TEMPLATE_FILES
  template/yarp_plugin_carrier.cpp.in
  template/yarp_plugin_RFModule.cpp.in
  template/yarp_plugin_device.cpp.in
  template/yarp_plugin_portmonitor.cpp.in
  template/yarp_plugin_library.cpp.in
  template/yarp_plugin_yarpdev_main.cpp.in
)

install(
  FILES ${YARP_TEMPLATE_FILES}
  DESTINATION share/yarp/cmake/template
  COMPONENT YARP_conf-dev
)

# Remove renamed files from installation
include(YarpRemoveFile)
foreach(_f
  YarpDeprecatedWarning.cmake   # since YARP 3.0.0
  deprecated/YarpRenamedOption.cmake
  deprecated/YarpBackupVariable.cmake
)
  yarp_remove_file("share/yarp/cmake/${_f}")
endforeach()

# Remove old YCM internal installations
foreach(_d
  ycm-0.6.0   # since YARP 3.0.0
  ycm-0.7.0
  ycm-0.8.0
  ycm-0.8.1   # since YARP 3.0.1
  ycm-0.8.20180628.11
  ycm-0.8.20180712.5
  ycm-0.8.20180720.3
  ycm-0.9.0   # since YARP 3.1.1
  ycm-0.9.0.8
  ycm-0.9.20181128.10
  ycm-0.9.20181204.8
  ycm-0.9.20190213.9
  ycm-0.9.20190315.16
  ycm-0.9.20190329.27
  ycm-0.10.0
  ycm-0.10.1
  ycm-0.10.2
  ycm-0.10.3
  ycm-0.11.0  # since YARP 3.3.0
)
  yarp_remove_directory("share/yarp/cmake/${_d}")
endforeach()
