# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

yarp_prepare_plugin(mjpeg
                    CATEGORY carrier
                    TYPE yarp::os::MjpegCarrier
                    INCLUDE MjpegCarrier.h
                    EXTRA_CONFIG CODE="GET /?ac"
                    DEPENDS "CREATE_OPTIONAL_CARRIERS;YARP_HAS_JPEG")

if(NOT SKIP_mjpeg)
  option(MJPEG_AUTOCOMPRESS "Automatically compress/decompress images on mjpeg stream" TRUE)
  if(MJPEG_AUTOCOMPRESS)
    add_definitions(-DMJPEG_AUTOCOMPRESS)
  endif()

  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_sig_INCLUDE_DIRS TARGET YARP_sig PROPERTY INCLUDE_DIRS)
  get_property(YARP_wire_rep_utils_INCLUDE_DIRS TARGET YARP_wire_rep_utils PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_sig_INCLUDE_DIRS}
                      ${YARP_wire_rep_utils_INCLUDE_DIRS})

  include_directories(SYSTEM ${JPEG_INCLUDE_DIR})

  yarp_add_plugin(yarp_mjpeg
                  MjpegCarrier.h
                  MjpegCarrier.cpp
                  MjpegStream.h
                  MjpegStream.cpp
                  MjpegDecompression.h
                  MjpegDecompression.cpp)
  target_link_libraries(yarp_mjpeg YARP_OS
                                   YARP_sig
                                   YARP_wire_rep_utils
                                   ${JPEG_LIBRARY})

  yarp_install(TARGETS yarp_mjpeg
               EXPORT YARP
               COMPONENT runtime
               LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
               ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR})
  yarp_install(FILES mjpeg.ini
               COMPONENT runtime
               DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR})

  if(YARP_COMPILE_TESTS)
    add_executable(test_mjpeg test_mjpeg.cpp
                              MjpegDecompression.cpp)
    target_link_libraries(test_mjpeg YARP_OS YARP_sig YARP_init)
    target_link_libraries(test_mjpeg ${JPEG_LIBRARY})
  endif()

endif()
