# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

include(CMakeDependentOption)
cmake_dependent_option(CREATE_YARPFS "Compile yarpfs executable" FALSE
                       YARP_COMPILE_UNMAINTAINED FALSE)
mark_as_advanced(CREATE_YARPFS)

if(CREATE_YARPFS)
  add_executable(yarpfs)

  set(yarpfs_SRCS yarpfs.cpp
                  link.cpp)
  set(yarpfs_HDRS yarpfns.h
                  yarputils.h)

  target_sources(yarpfs PRIVATE ${yarpfs_SRCS}
                                ${yarpfs_HDRS})

  target_link_libraries(yarpfs PRIVATE YARP::YARP_os
                                       YARP::YARP_init)

  target_compile_definitions(yarpfs PRIVATE ${Fuse_DEFINITIONS})
  target_include_directories(yarpfs PRIVATE ${Fuse_INCLUDE_DIRS})
  target_link_libraries(yarpfs PRIVATE ${Fuse_LIBRARIES})

  set_property(TARGET yarpfs PROPERTY FOLDER "Unsupported")
endif()
