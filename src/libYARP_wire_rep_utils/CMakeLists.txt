# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

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
  add_library(YARP::YARP_wire_rep_utils ALIAS YARP_wire_rep_utils)

  target_include_directories(YARP_wire_rep_utils PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                                                        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

  target_link_libraries(YARP_wire_rep_utils PRIVATE YARP::YARP_OS
                                                    YARP::YARP_sig
                                                    ${ACE_LIBRARIES})

  if(NOT CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.1)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
  endif()
  if(CMAKE_VERSION VERSION_LESS 3.1)
    if(DEFINED CXX11_FLAGS)
      target_compile_options(YARP_wire_rep_utils PUBLIC ${CXX11_FLAGS})
    endif()
  else()
    target_compile_features(YARP_wire_rep_utils PUBLIC cxx_nullptr
                                                       cxx_override)
  endif()

  set_property(TARGET YARP_wire_rep_utils PROPERTY INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
  set_property(GLOBAL APPEND PROPERTY YARP_LIBS YARP_wire_rep_utils)
  install(TARGETS YARP_wire_rep_utils
          EXPORT YARP
          COMPONENT runtime
          RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
          LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
          ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
  set_property(TARGET YARP_wire_rep_utils PROPERTY FOLDER "Libraries/Private")
endif()
