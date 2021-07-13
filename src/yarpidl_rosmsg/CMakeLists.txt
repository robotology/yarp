# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

include_directories(src)

set(md5_SRCS "${md5_ROOT}/md5.c")
set(md5_HDRS "${md5_ROOT}/md5.h")

set(yarpidl_rosmsg_SRCS
  src/main.cpp
  src/RosType.cpp
  src/RosTypeCodeGenYarp.cpp
)
set(yarpidl_rosmsg_HDRS
  src/RosType.h
  src/RosTypeCodeGenYarp.h
)

add_executable(yarpidl_rosmsg)
add_executable(YARP::yarpidl_rosmsg ALIAS yarpidl_rosmsg)

target_sources(yarpidl_rosmsg
  PRIVATE
    ${yarpidl_rosmsg_SRCS}
    ${yarpidl_rosmsg_HDRS}
    ${md5_SRCS}
    ${md5_HDRS}
)

target_include_directories(yarpidl_rosmsg SYSTEM PRIVATE "${md5_ROOT}")
target_link_libraries(yarpidl_rosmsg
  PRIVATE
    YARP::YARP_init
    YARP::YARP_os
)
if(YARP_HAS_ACE)
  target_compile_definitions(yarpidl_rosmsg PRIVATE YARP_HAS_ACE)
  target_link_libraries(yarpidl_rosmsg PRIVATE ACE::ACE)
endif()

install(
  TARGETS yarpidl_rosmsg
  EXPORT YARP_idl_tools
  COMPONENT YARP_idl_tools
  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set_property(TARGET yarpidl_rosmsg PROPERTY FOLDER "Command Line Tools")
