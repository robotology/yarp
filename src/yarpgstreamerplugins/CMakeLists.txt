# SPDX-FileCopyrightText: 2024 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

yarp_dependent_option  (YARP_COMPILE_gstreamerplugins "Do you want to compile gstreamerplugins?" OFF
  "YARP_HAS_GObject;YARP_HAS_GLIB2;YARP_HAS_GStreamer;YARP_HAS_GStreamerPluginsBase" OFF
)

if(YARP_COMPILE_gstreamerplugins)

add_subdirectory(videosource)
add_subdirectory(videosink)
add_subdirectory(videopassthrough)

endif()
