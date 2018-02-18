# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(CREATE_YARPMANAGER_CONSOLE)

  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_manager_INCLUDE_DIRS TARGET YARP_manager PROPERTY INCLUDE_DIRS)

  set(yarpmanager_console_SRCS yarpmanager.cpp
                               ymanager.cpp)
  set(yarpmanager_console_HDRS ymanager.h)

  include_directories(${YARP_OS_INCLUDE_DIRS})
  include_directories(${YARP_manager_INCLUDE_DIRS})
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})
  if(MSVC)
    include_directories(SYSTEM ${dirent_INCLUDE_DIRS})
  endif()

  if (YARP_HAS_LIBEDIT)
    add_definitions( -DWITH_LIBEDIT )
    include_directories(${Libedit_INCLUDE_DIR})
  endif()

  add_executable(yarpmanager-console ${yarpmanager_console_SRCS}
                                     ${yarpmanager_console_HDRS})

  target_link_libraries(yarpmanager-console YARP::YARP_init
                                            YARP::YARP_OS
                                            YARP::YARP_manager
                                            ${ACE_LIBRARIES})

  if(YARP_HAS_LIBEDIT)
    target_link_libraries(yarpmanager-console Libedit::libedit)
  endif()

  install(TARGETS yarpmanager-console
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpmanager-console PROPERTY FOLDER "Command Line Tools")
endif()
