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

  add_subdirectory(src)

  yarp_install(FILES yarpscope.xml DESTINATION ${YARP_MODULES_INSTALL_DIR})
  yarp_install(FILES examples/test_yarpscope_wbd.xml DESTINATION ${YARP_CONTEXTS_INSTALL_DIR}/yarpscope)

endif()
