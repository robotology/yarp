# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(YarpPlugin)
include(YarpPrintFeature)

yarp_begin_plugin_library(yarppm
  OPTION YARP_COMPILE_PORTMONITOR_PLUGINS
  DEFAULT ON
)
  add_subdirectory(depthimage_compression_zfp)
  add_subdirectory(depthimage_compression_zlib)
  add_subdirectory(depthimage_to_mono)
  add_subdirectory(depthimage_to_rgb)
  add_subdirectory(image_compression_ffmpeg)
  add_subdirectory(segmentationimage_to_rgb)
  add_subdirectory(sound_compression_mp3)
  add_subdirectory(soundfilter_resample)
yarp_end_plugin_library(yarppm QUIET)
add_library(YARP::yarppm ALIAS yarppm)

install(
  TARGETS yarppm
  EXPORT YARP_yarppm
  COMPONENT yarppm
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

include(YarpInstallBasicPackageFiles)
yarp_install_basic_package_files(YARP_yarppm
  DEPENDENCIES ${YARP_yarppm_PUBLIC_DEPS}
  PRIVATE_DEPENDENCIES ${YARP_yarppm_PRIVATE_DEPS}
)

set_property(TARGET yarppm PROPERTY FOLDER "Plugins/PortMonitors")

include(YarpDeprecatedOption)
