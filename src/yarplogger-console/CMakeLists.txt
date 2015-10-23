# Copyright: (C) 2014 RobotCub Consortium
# Authors: Marco Randazzo
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

if(CREATE_YARPLOGGER)

  set(PROJECTNAME yarplogger-console)
  get_property(YARP_OS_INCLUDE_DIRS TARGET YARP_OS PROPERTY INCLUDE_DIRS)
  get_property(YARP_logger_INCLUDE_DIRS TARGET YARP_logger PROPERTY INCLUDE_DIRS)

  include_directories(${YARP_OS_INCLUDE_DIRS}
                      ${YARP_logger_INCLUDE_DIRS})
  include_directories(SYSTEM ${ACE_INCLUDE_DIRS})

  add_executable(${PROJECTNAME} main.cpp)
  target_link_libraries(${PROJECTNAME} YARP_OS YARP_init YARP_logger)

  install(TARGETS ${PROJECTNAME} COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
