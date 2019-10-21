# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2006-2010 RobotCub Consortium
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

include(CMakeDependentOption)
cmake_dependent_option(CREATE_YARPFS "Compile yarpfs executable" FALSE
                       YARP_COMPILE_UNMAINTAINED FALSE)
mark_as_advanced(CREATE_YARPFS)

if(CREATE_YARPFS)
  # Find Fuse
  find_package(Fuse REQUIRED)
  add_definitions(${Fuse_DEFINITIONS})
  include_directories(${Fuse_INCLUDE_DIRS})

  set(yarpfs_SRCS yarpfs.cpp
                  link.cpp)
  set(yarpfs_HDRS yarpfns.h
                  yarputils.h)

  add_executable(yarpfs ${yarpfs_SRCS} ${yarpfs_HDRS})
  target_link_libraries(yarpfs YARP::YARP_os
                               YARP::YARP_init
                               ${Fuse_LIBRARIES})

  set_property(TARGET yarpfs PROPERTY FOLDER "Unsupported")
endif()
