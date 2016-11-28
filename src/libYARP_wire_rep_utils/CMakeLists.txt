# Copyright (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# used by tcpros_carrier, mjpeg_carrier

if(CREATE_OPTIONAL_CARRIERS)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})
  get_property(YARP_INCLUDE_DIRS GLOBAL PROPERTY YARP_TREE_INCLUDE_DIRS)
  include_directories(${YARP_INCLUDE_DIRS})
  add_library(YARP_wire_rep_utils STATIC WireImage.cpp
                                         WireImage.h
                                         WireBottle.cpp
                                         WireBottle.h
                                         WireTwiddler.cpp
                                         WireTwiddler.h)
  target_link_libraries(YARP_wire_rep_utils LINK_PRIVATE YARP_OS
                                                         YARP_sig
                                                         ${ACE_LIBRARIES})

  set_property(TARGET YARP_wire_rep_utils PROPERTY INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_wire_rep_utils)
  install(TARGETS YARP_wire_rep_utils
          EXPORT YARP
          COMPONENT runtime
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endif()
