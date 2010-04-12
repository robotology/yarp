# Copyright: (C) 2010 RobotCub Consortium
# Authors: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

if (CREATE_DEVICE_LIBRARY_MODULES)

  get_property(YARP_TREE_INCLUDE_DIRS TARGET YARP_dev PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_TREE_INCLUDE_DIRS})
  add_executable(yarphear yarphear.cpp)
  target_link_libraries(yarphear yarpmod)
  install(TARGETS yarphear COMPONENT utilities DESTINATION bin)

endif(CREATE_DEVICE_LIBRARY_MODULES)
