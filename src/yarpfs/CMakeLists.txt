# Copyright: (C) 2009 RobotCub Consortium
# Author: Paul Fitzpatrick
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT


include(CMakeDependentOption)
cmake_dependent_option(CREATE_YARPFS "Compile yarpfs executable" FALSE
                       YARP_COMPILE_UNMAINTAINED FALSE)
mark_as_advanced(CREATE_YARPFS)

if(CREATE_YARPFS)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  include_directories(${YARP_OS_INCLUDE_DIRS})

  include_directories(${ACE_INCLUDE_DIRS})

  # Find Fuse
  find_package(Fuse REQUIRED)
  add_definitions(${Fuse_DEFINITIONS})
  include_directories(${Fuse_INCLUDE_DIRS})

  set(yarpfs_SRCS yarpfs.cpp
                  link.cpp)
  set(yarpfs_HDRS yarpfns.h
                  yarputils.h)

  add_executable(yarpfs ${yarpfs_SRCS} ${yarpfs_HDRS})
  target_link_libraries(yarpfs YARP_OS
                               YARP_init
                               ${ACE_LIBRARIES}
                               ${Fuse_LIBRARIES})
endif()
