# Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

if(YARP_COMPILE_yarpmanager-console)
  set(yarpmanager_console_SRCS yarpmanager.cpp
                               ymanager.cpp)
  set(yarpmanager_console_HDRS ymanager.h)

  if(MSVC)
    include_directories(SYSTEM ${dirent_INCLUDE_DIRS})
  endif()

  add_executable(yarpmanager-console ${yarpmanager_console_SRCS}
                                     ${yarpmanager_console_HDRS})

  target_link_libraries(yarpmanager-console PRIVATE YARP::YARP_init
                                                    YARP::YARP_os
                                                    YARP::YARP_manager)

  if(YARP_HAS_Libedit)
    target_compile_definitions(yarpmanager-console PRIVATE YARP_HAS_Libedit)
    target_link_libraries(yarpmanager-console PRIVATE Libedit::libedit)
  endif()

  install(TARGETS yarpmanager-console
          COMPONENT utilities
          DESTINATION ${CMAKE_INSTALL_BINDIR})

  set_property(TARGET yarpmanager-console PROPERTY FOLDER "Command Line Tools")
endif()
