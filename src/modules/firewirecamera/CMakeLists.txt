# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if(UNIX)

  if(COMPILE_DEVICE_LIBRARY)
    yarp_prepare_device(firewirecamera TYPE yarp::dev::FirewireCamera INCLUDE FirewireCamera.h WRAPPER grabber)
    yarp_add_device_fingerprint(firewirecamera.ini firewirecamera)
  endif(COMPILE_DEVICE_LIBRARY)

  find_package(YARP REQUIRED)

  if(NOT SKIP_firewirecamera)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${YARP_INCLUDE_DIRS})
    find_package(YARP REQUIRED)
    message(STATUS "Warning, just taking it on faith that that raw1394 and dc1394_control exist")
    link_libraries(dc1394_control ${YARP_LIBRARIES})
    yarp_add_plugin(yarp_firewirecamera FirewireCamera.cpp FirewireCameraSet.cpp FirewireCameraSet.h dc1394settings.h FirewireCamera.h)
  endif(NOT SKIP_firewirecamera)

endif(UNIX)
