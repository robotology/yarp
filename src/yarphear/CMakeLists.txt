# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

if (CREATE_DEVICE_LIBRARY_MODULES)

  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  include_directories(${ACE_INCLUDE_DIRS})
  add_executable(yarphear yarphear.cpp)
  target_link_libraries(yarphear yarpmod YARP_init)

  if (ENABLE_FORCE_RPATH)
    set_target_properties( yarphear PROPERTIES INSTALL_RPATH ${FORCE_RPATH} )
  endif ()

  install(TARGETS yarphear COMPONENT utilities DESTINATION bin)

endif(CREATE_DEVICE_LIBRARY_MODULES)
