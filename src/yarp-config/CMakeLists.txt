# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause


set(yarp_config_SRCS
  yarpconfig.cpp
  yarpcontext.cpp
  yarprobot.cpp
  yarpcontextutils.cpp
)

set(yarp_config_HDRS
  yarpcontext.h
  yarprobot.h
  yarpcontextutils.h
)

add_executable(yarp-config)
target_sources(yarp-config
  PRIVATE
    ${yarp_config_SRCS}
    ${yarp_config_HDRS}
)

target_link_libraries(yarp-config
  PRIVATE
    YARP::YARP_os
    YARP::YARP_init
)

if(YARP_HAS_ACE)
  target_compile_definitions(yarp-config PRIVATE YARP_HAS_ACE)
  target_link_libraries(yarp-config PRIVATE ACE::ACE)
endif()

target_include_directories(yarp-config SYSTEM PRIVATE ${diff_match_patch_INCLUDE_DIRS})

install(
  TARGETS yarp-config
  COMPONENT utilities
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_property(TARGET yarp-config PROPERTY FOLDER "Command Line Tools")
