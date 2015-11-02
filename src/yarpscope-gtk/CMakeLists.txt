# Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
# Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


if(CREATE_YARPSCOPE AND YARP_HAS_GTK2)

  project(yarpscope-gtk)

  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(SYSTEM ${GTK2_INCLUDE_DIRS})
  include_directories(SYSTEM ${GtkDataboxMM_INCLUDE_DIRS})
  add_definitions(${GTK2_DEFINITIONS})

  # libsigc++ 2.5.1 or later requires c++11. This is automatically
  # enabled with CMake 3.1 or later (using FindGTK2.cmake from CMake
  # 3.5). With older versions, we enable it here.
  if(NOT ${CMAKE_MINIMUM_REQUIRED_VERSION} VERSION_LESS 3.1)
    message(AUTHOR_WARNING "CMAKE_MINIMUM_REQUIRED_VERSION is now ${CMAKE_MINIMUM_REQUIRED_VERSION}. This check can be removed.")
  endif()
  if(CMAKE_VERSION VERSION_LESS 3.1)
    if(NOT ${GTK2_SIGC++_VERSION_MAJOR}.${GTK2_SIGC++_VERSION_MINOR}.${GTK2_SIGC++_VERSION_MICRO} VERSION_LESS 2.5.1)
      add_definitions(${CXX11_FLAGS})
    endif()
  endif()


  add_subdirectory(src)

  yarp_install(FILES yarpscope.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})
  yarp_install(FILES examples/test_yarpscope_wbd.xml DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/yarpscope)

endif()
