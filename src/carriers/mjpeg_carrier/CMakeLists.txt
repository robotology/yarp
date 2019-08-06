# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

yarp_prepare_plugin(mjpeg
                    CATEGORY carrier
                    TYPE MjpegCarrier
                    INCLUDE MjpegCarrier.h
                    EXTRA_CONFIG CODE="GET /?ac"
                    DEPENDS "YARP_HAS_JPEG")

if(NOT SKIP_mjpeg)
  option(MJPEG_AUTOCOMPRESS "Automatically compress/decompress images on mjpeg stream" TRUE)
  if(MJPEG_AUTOCOMPRESS)
    add_definitions(-DMJPEG_AUTOCOMPRESS)
  endif()

  set(CMAKE_INCLUDE_CURRENT_DIR ON)


  yarp_add_plugin(yarp_mjpeg
                  MjpegCarrier.h
                  MjpegCarrier.cpp
                  MjpegStream.h
                  MjpegStream.cpp
                  MjpegDecompression.h
                  MjpegDecompression.cpp)
  target_link_libraries(yarp_mjpeg PRIVATE YARP::YARP_os
                                           YARP::YARP_sig
                                           YARP::YARP_wire_rep_utils)
  list(APPEND YARP_${YARP_PLUGIN_MASTER}_PRIVATE_DEPS YARP_os
                                                      YARP_sig
                                                      YARP_wire_rep_utils)

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
