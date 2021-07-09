# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

yarp_prepare_plugin(mjpeg
                    CATEGORY carrier
                    TYPE MjpegCarrier
                    INCLUDE MjpegCarrier.h
                    EXTRA_CONFIG CODE="GET /?ac"
                    DEPENDS "YARP_HAS_JPEG")

cmake_dependent_option(YARP_MJPEG_AUTOCOMPRESS "Automatically compress/decompress images on mjpeg stream" ON
                       "NOT SKIP_mjpeg" ON)
mark_as_advanced(YARP_MJPEG_AUTOCOMPRESS)
include(YarpRenamedOption)
yarp_renamed_option(MJPEG_AUTOCOMPRESS YARP_MJPEG_AUTOCOMPRESS) # Since YARP 3.3

if(NOT SKIP_mjpeg)
  yarp_add_plugin(yarp_mjpeg)

  target_sources(yarp_mjpeg PRIVATE MjpegCarrier.h
                                    MjpegCarrier.cpp
                                    MjpegStream.h
                                    MjpegStream.cpp
                                    MjpegDecompression.h
                                    MjpegDecompression.cpp
                                    MjpegLogComponent.h
                                    MjpegLogComponent.cpp)

  target_link_libraries(yarp_mjpeg PRIVATE YARP::YARP_os
                                           YARP::YARP_sig
                                           YARP::YARP_wire_rep_utils)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_wire_rep_utils)

  if(YARP_MJPEG_AUTOCOMPRESS)
    target_compile_definitions(yarp_mjpeg PRIVATE MJPEG_AUTOCOMPRESS)
  endif()

  target_include_directories(yarp_mjpeg SYSTEM PRIVATE ${JPEG_INCLUDE_DIR})
  target_link_libraries(yarp_mjpeg PRIVATE ${JPEG_LIBRARY})
#   list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS JPEG) (not using targets)

  yarp_install(TARGETS yarp_mjpeg
               EXPORT YARP_${YARP_PLUGIN_MASTER}
               COMPONENT ${YARP_PLUGIN_MASTER}
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
               YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  set(YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS ${YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS} PARENT_SCOPE)

  set_property(TARGET yarp_mjpeg PROPERTY FOLDER "Plugins/Carrier")
endif()
